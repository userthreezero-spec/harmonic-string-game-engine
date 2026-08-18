#!/usr/bin/env python3
"""
HSE-009 Ground-Truth Architecture Verifier (v2)

Robust C++ parser using regex-based pattern matching.
Parses headers and sources to extract classes, structs, members, enums, functions.

Usage:
    python verify.py --repo <path> --scan
    python verify.py --repo <path> --hse008
    python verify.py --repo <path> --benchmark
    python verify.py --repo <path> --claims <claims.json>
"""

import re
import os
import json
import sys
import argparse
from dataclasses import dataclass, field, asdict
from typing import List, Dict, Optional, Tuple
from enum import Enum


# ─── Classification Enums ───────────────────────────────────────────

class VerificationStatus(str, Enum):
    VERIFIED = "VERIFIED"
    CONTRADICTED = "CONTRADICTED"
    UNVERIFIED = "UNVERIFIED"
    INFERRED = "INFERRED"
    SUPPORTED = "SUPPORTED"
    NOT_FOUND = "NOT_FOUND"


class ClaimType(str, Enum):
    FILE_EXISTS = "file_exists"
    FILE_NOT_EXISTS = "file_not_exists"
    CLASS_EXISTS = "class_exists"
    STRUCT_EXISTS = "struct_exists"
    FUNCTION_EXISTS = "function_exists"
    MEMBER_EXISTS = "member_exists"
    ENUM_EXISTS = "enum_exists"
    INCLUDE_EXISTS = "include_exists"
    NAMESPACE_EXISTS = "namespace_exists"
    BUILD_TARGET_EXISTS = "build_target_exists"
    RELATIONSHIP = "relationship"


# ─── Data Classes ───────────────────────────────────────────────────

@dataclass
class Claim:
    claim_id: str
    claim_type: str
    description: str = ""
    file_path: str = ""
    symbol_name: str = ""
    symbol_type: str = ""
    owner_class: str = ""
    namespace: str = ""
    relationship: str = ""
    source_symbol: str = ""
    target_symbol: str = ""
    expected: str = ""


@dataclass
class VerificationResult:
    claim_id: str
    status: str
    evidence: str
    location: str = ""
    details: str = ""


# ─── Robust C++ Parser ──────────────────────────────────────────────

class CppParser:
    """Parses C++ files using regex-based pattern matching."""

    def __init__(self, repo_root: str):
        self.repo_root = repo_root
        self.files: List[str] = []
        self.classes: Dict[str, Dict] = {}
        self.structs: Dict[str, Dict] = {}
        self.enums: List[Dict] = []
        self.functions: List[Dict] = []
        self.includes: List[Dict] = []
        self.namespaces: set = set()

    def _rel(self, path: str) -> str:
        """Convert to forward-slash relative path."""
        return os.path.relpath(path, self.repo_root).replace('\\', '/')

    def scan(self):
        """Scan all .h and .cpp files."""
        for root, dirs, files in os.walk(self.repo_root):
            if 'build' in root or '.git' in root:
                continue
            for f in files:
                if f.endswith(('.h', '.cpp', '.hpp')):
                    filepath = os.path.join(root, f)
                    self.files.append(self._rel(filepath))
                    self._parse_file(filepath)

    def _parse_file(self, filepath: str):
        """Parse a single file with robust regex patterns."""
        rel = self._rel(filepath)
        
        try:
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
        except Exception:
            return

        lines = content.split('\n')
        current_ns = ""
        current_class = ""
        in_class = False
        brace_depth = 0
        class_brace_start = 0

        for i, line in enumerate(lines, 1):
            stripped = line.strip()
            
            # Skip comments
            if stripped.startswith('//') or stripped.startswith('/*') or stripped.startswith('*'):
                continue

            # Track namespace
            ns_m = re.match(r'namespace\s+(\w+)', stripped)
            if ns_m:
                current_ns = ns_m.group(1)
                self.namespaces.add(current_ns)

            # Track includes
            inc_m = re.match(r'#include\s+[<"]([^>"]+)[>"]', stripped)
            if inc_m:
                self.includes.append({
                    'source_file': rel,
                    'target': inc_m.group(1),
                    'line': i
                })

            # Detect class/struct definition (header)
            if filepath.endswith('.h') or filepath.endswith('.hpp'):
                # Pattern: class Name { or class Name : public Base {
                cs_m = re.match(r'(class|struct)\s+(\w+)\s*(?::\s*(?:public|private|protected)\s+([\w:]+))?\s*\{', stripped)
                if cs_m and 'enum' not in stripped:
                    kind = cs_m.group(1)
                    name = cs_m.group(2)
                    base = cs_m.group(3) or ""
                    
                    # Extract the class body
                    body = self._extract_class_body(content, content.find(name + ' ') or content.find(name + '\n'))
                    
                    if kind == 'class':
                        self.classes[name] = self._parse_class(name, current_ns, rel, "", i, body, base)
                    else:
                        self.structs[name] = self._parse_struct(name, current_ns, rel, i, body)

            # Detect class/struct definition (source - for methods)
            if filepath.endswith('.cpp'):
                cs_m = re.match(r'(class|struct)\s+(\w+)\s*(?::\s*(?:public|private|protected)\s+([\w:]+))?\s*\{', stripped)
                if cs_m and 'enum' not in stripped:
                    kind = cs_m.group(1)
                    name = cs_m.group(2)
                    
                    body = self._extract_class_body(content, content.find(name + ' ') or content.find(name + '\n'))
                    
                    if kind == 'class' and name in self.classes:
                        # Merge source file info
                        self.classes[name]['source_file'] = rel
                        # Parse methods from source
                        methods = self._parse_methods(body, name, current_ns, rel)
                        self.classes[name]['methods'].extend(methods)
                    elif kind == 'struct' and name in self.structs:
                        self.structs[name]['source_file'] = rel

            # Detect enum
            enum_m = re.match(r'enum\s+(?:class\s+)?(\w+)\s*\{', stripped)
            if enum_m:
                name = enum_m.group(1)
                # Extract enum values
                values = self._extract_enum_values(content, content.find('enum ' + name) or content.find('enum class ' + name))
                self.enums.append({
                    'name': name,
                    'namespace': current_ns,
                    'header_file': rel if filepath.endswith('.h') else "",
                    'values': values,
                    'line': i
                })

            # Detect standalone functions in source files
            if filepath.endswith('.cpp'):
                func_m = re.match(r'(?:[\w:*&<>\s]+?)\s+(\w+(?:::\w+)?)\s*\([^)]*\)\s*(?:const)?\s*\{', stripped)
                if func_m:
                    fname = func_m.group(1)
                    if fname not in ('if', 'for', 'while', 'switch', 'return', 'sizeof', 'namespace', 'case'):
                        self.functions.append({
                            'name': fname,
                            'owner': "",
                            'namespace': current_ns,
                            'signature': stripped[:120],
                            'header_file': "",
                            'source_file': rel,
                            'definition_line': i
                        })

    def _extract_class_body(self, content: str, start: int) -> str:
        """Extract class body between { and matching }."""
        if start < 0:
            return ""
        
        # Find the opening brace
        brace_pos = content.find('{', start)
        if brace_pos < 0:
            return ""
        
        depth = 0
        body = []
        for i in range(brace_pos, len(content)):
            if content[i] == '{':
                depth += 1
            elif content[i] == '}':
                depth -= 1
                if depth == 0:
                    break
            if depth > 0:
                body.append(content[i])
        
        return ''.join(body)

    def _parse_class(self, name: str, ns: str, header: str, source: str, line: int, body: str, base: str) -> Dict:
        """Parse a class from its body text."""
        fields = []
        methods = []
        
        lines = body.split('\n')
        access = 'private'  # default for class
        
        for l in lines:
            s = l.strip()
            
            # Track access specifiers
            if s.startswith('public:'):
                access = 'public'
                continue
            elif s.startswith('private:'):
                access = 'private'
                continue
            elif s.startswith('protected:'):
                access = 'protected'
                continue
            
            # Skip empty, comments, preprocessor
            if not s or s.startswith('//') or s.startswith('#') or s.startswith('/*') or s.startswith('*'):
                continue
            
            # Skip access-only lines
            if s in ('public', 'private', 'protected'):
                continue
            
            # Detect fields: type name; or type name = value; or type name{value};
            # Must NOT contain '(' (that's a method)
            if ';' in s and '(' not in s:
                # Try to match: [static] [const] type name [= value];
                # Type can be multi-word like "unsigned int", "const std::string"
                field_m = re.match(r'(?:static\s+)?(?:const\s+)?((?:unsigned\s+|signed\s+)?[\w:*&<>\[\]]+(?:\s+[\w:*&<>\[\]]+)*)\s+(\w+)\s*[;={]', s)
                if field_m:
                    ftype = field_m.group(1)
                    fname = field_m.group(2)
                    # Skip known non-field tokens
                    if fname not in ('class', 'struct', 'enum', 'return', 'if', 'for', 'while'):
                        fields.append({
                            'name': fname,
                            'type': ftype,
                            'access': access,
                            'line': 0
                        })
            
            # Detect methods: return_type name(args) { or ;
            elif '(' in s and ('{' in s or ';' in s):
                method_m = re.match(r'(?:static\s+)?(?:virtual\s+)?(?:inline\s+)?(?:constexpr\s+)?([\w:*&<>\s]+?)\s+(\w+)\s*\(', s)
                if method_m:
                    mname = method_m.group(2)
                    if mname not in ('if', 'for', 'while', 'switch', 'return', 'sizeof', 'class', 'struct'):
                        methods.append({
                            'name': mname,
                            'signature': s[:120],
                            'access': access,
                            'line': 0
                        })
            
            # Detect inline methods (no semicolon, no brace): return_type name(args) const
            elif '(' in s and not s.endswith(';') and not s.endswith('{'):
                method_m = re.match(r'(?:static\s+)?(?:virtual\s+)?([\w:*&<>\s]+?)\s+(\w+)\s*\([^)]*\)\s*(?:const)?\s*$', s)
                if method_m:
                    mname = method_m.group(2)
                    if mname not in ('if', 'for', 'while', 'switch', 'return', 'sizeof'):
                        methods.append({
                            'name': mname,
                            'signature': s[:120],
                            'access': access,
                            'line': 0
                        })
        
        return {
            'name': name,
            'namespace': ns,
            'header_file': header,
            'source_file': source,
            'line': line,
            'fields': fields,
            'methods': methods,
            'base_classes': [base] if base else []
        }

    def _parse_struct(self, name: str, ns: str, header: str, line: int, body: str) -> Dict:
        """Parse a struct from its body text."""
        fields = []
        
        lines = body.split('\n')
        for l in lines:
            s = l.strip()
            if not s or s.startswith('//') or s.startswith('#'):
                continue
            
            # Structs are public by default
            if ';' in s and '(' not in s:
                field_m = re.match(r'(?:static\s+)?(?:const\s+)?((?:unsigned\s+|signed\s+)?[\w:*&<>\[\]]+(?:\s+[\w:*&<>\[\]]+)*)\s+(\w+)\s*[;={]', s)
                if field_m:
                    ftype = field_m.group(1)
                    fname = field_m.group(2)
                    if fname not in ('class', 'struct', 'enum', 'return'):
                        fields.append({
                            'name': fname,
                            'type': ftype,
                            'access': 'public',
                            'line': 0
                        })
        
        return {
            'name': name,
            'namespace': ns,
            'header_file': header,
            'source_file': "",
            'line': line,
            'fields': fields
        }

    def _parse_methods(self, body: str, class_name: str, ns: str, source: str) -> List[Dict]:
        """Parse methods from a class body (typically in .cpp file)."""
        methods = []
        lines = body.split('\n')
        
        for l in lines:
            s = l.strip()
            if not s or s.startswith('//') or s.startswith('#'):
                continue
            
            # Method definition: ClassName::MethodName(args) {
            method_m = re.match(r'(?:[\w:*&<>\s]+?)\s+' + re.escape(class_name) + r'::(\w+)\s*\([^)]*\)', s)
            if method_m:
                mname = method_m.group(1)
                methods.append({
                    'name': mname,
                    'signature': s[:120],
                    'access': 'public',
                    'line': 0,
                    'source_file': source
                })
        
        return methods

    def _extract_enum_values(self, content: str, start: int) -> List[str]:
        """Extract enum values."""
        if start < 0:
            return []
        
        # Find opening brace
        brace_pos = content.find('{', start)
        if brace_pos < 0:
            return []
        
        # Find closing brace
        depth = 0
        values = []
        for i in range(brace_pos, min(brace_pos + 500, len(content))):
            if content[i] == '{':
                depth += 1
            elif content[i] == '}':
                depth -= 1
                if depth == 0:
                    break
            elif depth == 1:
                # Inside the enum
                pass
        
        # Extract text between braces
        enum_text = content[brace_pos:i] if i > brace_pos else ""
        # Find values
        for line in enum_text.split('\n'):
            line = line.strip().rstrip(',')
            if line and not line.startswith('{') and not line.startswith('}') and not line.startswith('//'):
                val = line.split('=')[0].strip()
                if val and val not in ('', '{', '}', 'enum', 'class'):
                    values.append(val)
        
        return values

    def get_ground_truth(self) -> Dict:
        """Return the complete ground truth model."""
        return {
            'files': sorted(self.files),
            'namespaces': sorted(self.namespaces),
            'classes': self.classes,
            'structs': self.structs,
            'enums': self.enums,
            'functions': self.functions,
            'includes': self.includes
        }


# ─── Verifier ───────────────────────────────────────────────────────

class ArchitectureVerifier:
    """Verifies architectural claims against ground truth."""

    def __init__(self, ground_truth: Dict):
        self.gt = ground_truth

    def verify_claim(self, claim: Claim) -> VerificationResult:
        """Verify a single claim."""
        
        if claim.claim_type == ClaimType.FILE_EXISTS:
            return self._verify_file_exists(claim)
        elif claim.claim_type == ClaimType.FILE_NOT_EXISTS:
            return self._verify_file_not_exists(claim)
        elif claim.claim_type == ClaimType.CLASS_EXISTS:
            return self._verify_class_exists(claim)
        elif claim.claim_type == ClaimType.STRUCT_EXISTS:
            return self._verify_struct_exists(claim)
        elif claim.claim_type == ClaimType.FUNCTION_EXISTS:
            return self._verify_function_exists(claim)
        elif claim.claim_type == ClaimType.MEMBER_EXISTS:
            return self._verify_member_exists(claim)
        elif claim.claim_type == ClaimType.ENUM_EXISTS:
            return self._verify_enum_exists(claim)
        elif claim.claim_type == ClaimType.INCLUDE_EXISTS:
            return self._verify_include_exists(claim)
        elif claim.claim_type == ClaimType.NAMESPACE_EXISTS:
            return self._verify_namespace_exists(claim)
        elif claim.claim_type == ClaimType.RELATIONSHIP:
            return self._verify_relationship(claim)
        else:
            return VerificationResult(
                claim_id=claim.claim_id,
                status=VerificationStatus.UNVERIFIED.value,
                evidence=f"Unknown claim type: {claim.claim_type}"
            )

    def _normalize_path(self, path: str) -> str:
        """Normalize path to forward slashes, no leading ."""
        p = path.replace('\\', '/')
        if p.startswith('./'):
            p = p[2:]
        return p

    def _verify_file_exists(self, claim: Claim) -> VerificationResult:
        """Verify file exists."""
        path = self._normalize_path(claim.file_path)
        
        for f in self.gt['files']:
            if self._normalize_path(f) == path:
                return VerificationResult(
                    claim_id=claim.claim_id,
                    status=VerificationStatus.VERIFIED.value,
                    evidence=f"File found: {f}",
                    location=f
                )
        
        return VerificationResult(
            claim_id=claim.claim_id,
            status=VerificationStatus.CONTRADICTED.value,
            evidence=f"File NOT found: {path}"
        )

    def _verify_file_not_exists(self, claim: Claim) -> VerificationResult:
        """Verify file does NOT exist."""
        path = self._normalize_path(claim.file_path)
        
        for f in self.gt['files']:
            if self._normalize_path(f) == path:
                return VerificationResult(
                    claim_id=claim.claim_id,
                    status=VerificationStatus.CONTRADICTED.value,
                    evidence=f"File unexpectedly found: {f}"
                )
        
        return VerificationResult(
            claim_id=claim.claim_id,
            status=VerificationStatus.VERIFIED.value,
            evidence=f"File correctly not found: {path}"
        )

    def _verify_class_exists(self, claim: Claim) -> VerificationResult:
        """Verify class exists."""
        name = claim.symbol_name
        
        if name in self.gt['classes']:
            c = self.gt['classes'][name]
            loc = c.get('header_file') or c.get('source_file', '')
            return VerificationResult(
                claim_id=claim.claim_id,
                status=VerificationStatus.VERIFIED.value,
                evidence=f"Class '{name}' found in {loc}:{c['line']}",
                location=f"{loc}:{c['line']}"
            )
        
        return VerificationResult(
            claim_id=claim.claim_id,
            status=VerificationStatus.CONTRADICTED.value,
            evidence=f"Class '{name}' NOT found in repository"
        )

    def _verify_struct_exists(self, claim: Claim) -> VerificationResult:
        """Verify struct exists."""
        name = claim.symbol_name
        
        if name in self.gt['structs']:
            s = self.gt['structs'][name]
            loc = s.get('header_file', '')
            return VerificationResult(
                claim_id=claim.claim_id,
                status=VerificationStatus.VERIFIED.value,
                evidence=f"Struct '{name}' found in {loc}:{s['line']}",
                location=f"{loc}:{s['line']}"
            )
        
        return VerificationResult(
            claim_id=claim.claim_id,
            status=VerificationStatus.CONTRADICTED.value,
            evidence=f"Struct '{name}' NOT found in repository"
        )

    def _verify_function_exists(self, claim: Claim) -> VerificationResult:
        """Verify function exists."""
        name = claim.symbol_name
        owner = claim.owner_class
        
        # Check standalone functions
        for f in self.gt['functions']:
            if f['name'] == name and (not owner or f['owner'] == owner):
                loc = f['source_file'] or f.get('header_file', '')
                return VerificationResult(
                    claim_id=claim.claim_id,
                    status=VerificationStatus.VERIFIED.value,
                    evidence=f"Function '{name}' found in {loc}:{f['definition_line']}"
                )
        
        # Check class methods
        if owner and owner in self.gt['classes']:
            for m in self.gt['classes'][owner]['methods']:
                if m['name'] == name:
                    loc = self.gt['classes'][owner].get('source_file', '') or self.gt['classes'][owner].get('header_file', '')
                    return VerificationResult(
                        claim_id=claim.claim_id,
                        status=VerificationStatus.VERIFIED.value,
                        evidence=f"Method '{owner}::{name}' found in {loc}"
                    )
        
        # Check all classes if no owner specified
        if not owner:
            for cname, c in self.gt['classes'].items():
                for m in c['methods']:
                    if m['name'] == name:
                        loc = c.get('source_file', '') or c.get('header_file', '')
                        return VerificationResult(
                            claim_id=claim.claim_id,
                            status=VerificationStatus.VERIFIED.value,
                            evidence=f"Method '{cname}::{name}' found in {loc}"
                        )
        
        return VerificationResult(
            claim_id=claim.claim_id,
            status=VerificationStatus.CONTRADICTED.value,
            evidence=f"Function '{name}' NOT found" + (f" in class '{owner}'" if owner else "")
        )

    def _verify_member_exists(self, claim: Claim) -> VerificationResult:
        """Verify member exists."""
        name = claim.symbol_name
        owner = claim.owner_class
        
        if not owner:
            return VerificationResult(
                claim_id=claim.claim_id,
                status=VerificationStatus.UNVERIFIED.value,
                evidence="Member claim requires owner_class"
            )
        
        # Check class fields
        if owner in self.gt['classes']:
            for f in self.gt['classes'][owner]['fields']:
                if f['name'] == name:
                    loc = self.gt['classes'][owner].get('header_file', '') or self.gt['classes'][owner].get('source_file', '')
                    return VerificationResult(
                        claim_id=claim.claim_id,
                        status=VerificationStatus.VERIFIED.value,
                        evidence=f"Member '{owner}::{name}' found (type: {f['type']}) in {loc}",
                        location=loc
                    )
            
            # Owner exists but member doesn't
            member_names = [f['name'] for f in self.gt['classes'][owner]['fields']]
            return VerificationResult(
                claim_id=claim.claim_id,
                status=VerificationStatus.CONTRADICTED.value,
                evidence=f"Member '{owner}::{name}' NOT found. {owner} has {len(member_names)} members: {', '.join(member_names[:10])}"
            )
        
        # Check struct fields
        if owner in self.gt['structs']:
            for f in self.gt['structs'][owner]['fields']:
                if f['name'] == name:
                    loc = self.gt['structs'][owner].get('header_file', '')
                    return VerificationResult(
                        claim_id=claim.claim_id,
                        status=VerificationStatus.VERIFIED.value,
                        evidence=f"Member '{owner}::{name}' found in {loc}",
                        location=loc
                    )
            
            member_names = [f['name'] for f in self.gt['structs'][owner]['fields']]
            return VerificationResult(
                claim_id=claim.claim_id,
                status=VerificationStatus.CONTRADICTED.value,
                evidence=f"Member '{owner}::{name}' NOT found. {owner} has {len(member_names)} members: {', '.join(member_names[:10])}"
            )
        
        return VerificationResult(
            claim_id=claim.claim_id,
            status=VerificationStatus.UNVERIFIED.value,
            evidence=f"Owner '{owner}' not found, cannot verify member '{name}'"
        )

    def _verify_enum_exists(self, claim: Claim) -> VerificationResult:
        """Verify enum exists."""
        name = claim.symbol_name
        
        for e in self.gt['enums']:
            if e['name'] == name:
                return VerificationResult(
                    claim_id=claim.claim_id,
                    status=VerificationStatus.VERIFIED.value,
                    evidence=f"Enum '{name}' found with values: {', '.join(e['values'][:5])}",
                    location=f"{e['header_file']}:{e['line']}"
                )
        
        return VerificationResult(
            claim_id=claim.claim_id,
            status=VerificationStatus.CONTRADICTED.value,
            evidence=f"Enum '{name}' NOT found in repository"
        )

    def _verify_include_exists(self, claim: Claim) -> VerificationResult:
        """Verify include exists."""
        source = self._normalize_path(claim.source_symbol)
        target = claim.target_symbol
        
        for inc in self.gt['includes']:
            if self._normalize_path(inc['source_file']) == source and inc['target'] == target:
                return VerificationResult(
                    claim_id=claim.claim_id,
                    status=VerificationStatus.VERIFIED.value,
                    evidence=f"Include '{source}' -> '{target}' found at line {inc['line']}"
                )
        
        return VerificationResult(
            claim_id=claim.claim_id,
            status=VerificationStatus.CONTRADICTED.value,
            evidence=f"Include '{source}' -> '{target}' NOT found"
        )

    def _verify_namespace_exists(self, claim: Claim) -> VerificationResult:
        """Verify namespace exists."""
        name = claim.symbol_name
        
        if name in self.gt['namespaces']:
            return VerificationResult(
                claim_id=claim.claim_id,
                status=VerificationStatus.VERIFIED.value,
                evidence=f"Namespace '{name}' found"
            )
        
        return VerificationResult(
            claim_id=claim.claim_id,
            status=VerificationStatus.CONTRADICTED.value,
            evidence=f"Namespace '{name}' NOT found"
        )

    def _verify_relationship(self, claim: Claim) -> VerificationResult:
        """Verify relationship."""
        rel = claim.relationship
        source = claim.source_symbol
        target = claim.target_symbol
        
        if rel == "owns":
            # Check if source class/struct has target as member
            if source in self.gt['classes']:
                for f in self.gt['classes'][source]['fields']:
                    if f['name'] == target:
                        return VerificationResult(
                            claim_id=claim.claim_id,
                            status=VerificationStatus.VERIFIED.value,
                            evidence=f"{source} owns {target} (type: {f['type']})"
                        )
                return VerificationResult(
                    claim_id=claim.claim_id,
                    status=VerificationStatus.CONTRADICTED.value,
                    evidence=f"{source} does NOT own {target}"
                )
            elif source in self.gt['structs']:
                for f in self.gt['structs'][source]['fields']:
                    if f['name'] == target:
                        return VerificationResult(
                            claim_id=claim.claim_id,
                            status=VerificationStatus.VERIFIED.value,
                            evidence=f"{source} owns {target}"
                        )
                return VerificationResult(
                    claim_id=claim.claim_id,
                    status=VerificationStatus.CONTRADICTED.value,
                    evidence=f"{source} does NOT own {target}"
                )
            else:
                return VerificationResult(
                    claim_id=claim.claim_id,
                    status=VerificationStatus.UNVERIFIED.value,
                    evidence=f"Source '{source}' not found"
                )
        
        return VerificationResult(
            claim_id=claim.claim_id,
            status=VerificationStatus.UNVERIFIED.value,
            evidence=f"Relationship type '{rel}' not yet implemented"
        )

    def verify_all(self, claims: List[Claim]) -> List[VerificationResult]:
        """Verify all claims."""
        return [self.verify_claim(c) for c in claims]


# ─── Claim Sets ─────────────────────────────────────────────────────

def build_hse008_texture_claims() -> List[Claim]:
    """HSE-008 texture mission claims — the known test cases."""
    claims = []
    cid = 0

    def add(ctype, **kwargs):
        nonlocal cid
        claims.append(Claim(claim_id=f"C{cid:03d}", claim_type=ctype, **kwargs))
        cid += 1

    # === VALID CLAIMS (should be VERIFIED) ===
    add(ClaimType.FILE_EXISTS, description="Primitive header exists", file_path="include/scene/primitive.h")
    add(ClaimType.FILE_EXISTS, description="Renderer source exists", file_path="src/renderer/renderer.cpp")
    add(ClaimType.FILE_EXISTS, description="Scene builder source exists", file_path="src/scene/scene_builder.cpp")
    add(ClaimType.FILE_EXISTS, description="Bridge source exists", file_path="src/bridge/bridge.cpp")
    add(ClaimType.FILE_EXISTS, description="Command queue header exists", file_path="include/bridge/command_queue.h")
    add(ClaimType.CLASS_EXISTS, description="Primitive class exists", symbol_name="Primitive", symbol_type="class")
    add(ClaimType.CLASS_EXISTS, description="Renderer class exists", symbol_name="Renderer", symbol_type="class")
    add(ClaimType.CLASS_EXISTS, description="Scene class exists", symbol_name="Scene", symbol_type="class")
    add(ClaimType.MEMBER_EXISTS, description="Primitive has m_color", symbol_name="m_color", symbol_type="member", owner_class="Primitive")
    add(ClaimType.MEMBER_EXISTS, description="Primitive has m_vertices", symbol_name="m_vertices", symbol_type="member", owner_class="Primitive")
    add(ClaimType.MEMBER_EXISTS, description="Primitive has m_vao", symbol_name="m_vao", symbol_type="member", owner_class="Primitive")
    add(ClaimType.MEMBER_EXISTS, description="Primitive has m_id", symbol_name="m_id", symbol_type="member", owner_class="Primitive")
    add(ClaimType.ENUM_EXISTS, description="PrimitiveType enum exists", symbol_name="PrimitiveType", symbol_type="enum")
    add(ClaimType.NAMESPACE_EXISTS, description="hse namespace exists", symbol_name="hse", symbol_type="namespace")

    # === INVALID CLAIMS (should be CONTRADICTED) ===
    add(ClaimType.MEMBER_EXISTS, description="HSE-008 ERROR A: m_normals does NOT exist",
        symbol_name="m_normals", symbol_type="member", owner_class="Primitive")
    add(ClaimType.FILE_EXISTS, description="HSE-008 ERROR B: renderer_state.cpp does NOT exist",
        file_path="src/renderer/renderer_state.cpp")
    add(ClaimType.MEMBER_EXISTS, description="INVALID: m_textureID does not exist yet",
        symbol_name="m_textureID", symbol_type="member", owner_class="Primitive")
    add(ClaimType.MEMBER_EXISTS, description="INVALID: m_texCoords does not exist yet",
        symbol_name="m_texCoords", symbol_type="member", owner_class="Primitive")
    add(ClaimType.CLASS_EXISTS, description="INVALID: TextureManager does not exist yet",
        symbol_name="TextureManager", symbol_type="class")
    add(ClaimType.FILE_EXISTS, description="INVALID: texture_manager.cpp does not exist yet",
        file_path="src/resource/texture_manager.cpp")
    add(ClaimType.MEMBER_EXISTS, description="INVALID: m_uvbo does not exist yet",
        symbol_name="m_uvbo", symbol_type="member", owner_class="Primitive")

    # === OWNERSHIP CLAIMS ===
    add(ClaimType.RELATIONSHIP, description="Scene owns m_primitives",
        relationship="owns", source_symbol="Scene", target_symbol="m_primitives")
    add(ClaimType.RELATIONSHIP, description="Primitive owns m_vao",
        relationship="owns", source_symbol="Primitive", target_symbol="m_vao")
    add(ClaimType.RELATIONSHIP, description="INVALID: Primitive owns m_normals",
        relationship="owns", source_symbol="Primitive", target_symbol="m_normals")

    return claims


def build_symbol_benchmark() -> List[Claim]:
    """10 valid + 5 invalid claims for accuracy measurement."""
    claims = []
    cid = 100

    def add(ctype, **kwargs):
        nonlocal cid
        claims.append(Claim(claim_id=f"C{cid:03d}", claim_type=ctype, **kwargs))
        cid += 1

    # 10 VALID
    add(ClaimType.FILE_EXISTS, description="Camera header", file_path="include/scene/camera.h")
    add(ClaimType.CLASS_EXISTS, description="Camera class", symbol_name="Camera", symbol_type="class")
    add(ClaimType.CLASS_EXISTS, description="Bridge class", symbol_name="Bridge", symbol_type="class")
    add(ClaimType.MEMBER_EXISTS, description="Primitive::m_position", symbol_name="m_position", symbol_type="member", owner_class="Primitive")
    add(ClaimType.MEMBER_EXISTS, description="Camera::m_fovDeg", symbol_name="m_fovDeg", symbol_type="member", owner_class="Camera")
    add(ClaimType.MEMBER_EXISTS, description="Bridge::m_running", symbol_name="m_running", symbol_type="member", owner_class="Bridge")
    add(ClaimType.ENUM_EXISTS, description="ProjectionType", symbol_name="ProjectionType", symbol_type="enum")
    add(ClaimType.FUNCTION_EXISTS, description="Primitive::uploadGPU", symbol_name="uploadGPU", symbol_type="function", owner_class="Primitive")
    add(ClaimType.FUNCTION_EXISTS, description="Renderer::renderScene", symbol_name="renderScene", symbol_type="function", owner_class="Renderer")
    add(ClaimType.NAMESPACE_EXISTS, description="hse namespace", symbol_name="hse", symbol_type="namespace")

    # 5 INVALID
    add(ClaimType.MEMBER_EXISTS, description="INVALID: Primitive::m_normals",
        symbol_name="m_normals", symbol_type="member", owner_class="Primitive")
    add(ClaimType.FILE_EXISTS, description="INVALID: renderer_state.cpp",
        file_path="src/renderer/renderer_state.cpp")
    add(ClaimType.CLASS_EXISTS, description="INVALID: TextureManager",
        symbol_name="TextureManager", symbol_type="class")
    add(ClaimType.MEMBER_EXISTS, description="INVALID: Primitive::m_textureID",
        symbol_name="m_textureID", symbol_type="member", owner_class="Primitive")
    add(ClaimType.FILE_EXISTS, description="INVALID: texture_manager.cpp",
        file_path="src/resource/texture_manager.cpp")

    return claims


# ─── Main ───────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(description='HSE-009 Ground-Truth Architecture Verifier')
    parser.add_argument('--repo', required=True, help='Path to HSE repository')
    parser.add_argument('--claims', help='Path to claims JSON file')
    parser.add_argument('--output', default='verification_results.json', help='Output file')
    parser.add_argument('--scan', action='store_true', help='Scan and dump ground truth')
    parser.add_argument('--hse008', action='store_true', help='Run HSE-008 regression')
    parser.add_argument('--benchmark', action='store_true', help='Run symbol accuracy benchmark')
    args = parser.parse_args()

    # Parse repository
    print(f"Parsing repository: {args.repo}")
    cpp_parser = CppParser(args.repo)
    cpp_parser.scan()
    gt = cpp_parser.get_ground_truth()

    print(f"  Files: {len(gt['files'])}")
    print(f"  Classes: {len(gt['classes'])}")
    print(f"  Structs: {len(gt['structs'])}")
    print(f"  Enums: {len(gt['enums'])}")
    print(f"  Functions: {len(gt['functions'])}")
    print(f"  Includes: {len(gt['includes'])}")
    print(f"  Namespaces: {len(gt['namespaces'])}")

    # Show class details
    for name, c in gt['classes'].items():
        fields = [f['name'] for f in c['fields']]
        methods = [m['name'] for m in c['methods']]
        print(f"    {name}: {len(fields)} fields [{', '.join(fields[:5])}], {len(methods)} methods")

    if args.scan:
        output_path = os.path.join(args.repo, args.output)
        with open(output_path, 'w') as f:
            json.dump(gt, f, indent=2)
        print(f"\nGround truth written to: {output_path}")
        return

    # Load claims
    if args.hse008:
        claims = build_hse008_texture_claims()
        print(f"\nHSE-008 texture mission claims: {len(claims)}")
    elif args.benchmark:
        claims = build_symbol_benchmark()
        print(f"\nSymbol accuracy benchmark: {len(claims)}")
    elif args.claims:
        with open(args.claims) as f:
            claims = [Claim(**c) for c in json.load(f)]
        print(f"\nLoaded claims: {len(claims)}")
    else:
        print("No claims specified. Use --hse008, --benchmark, or --claims")
        return

    # Verify
    verifier = ArchitectureVerifier(gt)
    results = verifier.verify_all(claims)

    # Count
    vc = sum(1 for r in results if r.status == "VERIFIED")
    cc = sum(1 for r in results if r.status == "CONTRADICTED")
    uc = sum(1 for r in results if r.status == "UNVERIFIED")
    ic = sum(1 for r in results if r.status == "INFERRED")
    total = len(results)

    # Report
    print(f"\n{'='*60}")
    print(f"VERIFICATION RESULTS")
    print(f"{'='*60}")
    print(f"Total:     {total}")
    print(f"VERIFIED:      {vc}  ({100*vc//total if total else 0}%)")
    print(f"CONTRADICTED:  {cc}  ({100*cc//total if total else 0}%)")
    print(f"UNVERIFIED:    {uc}")
    print(f"INFERRED:      {ic}")
    print(f"{'='*60}")

    for r in results:
        sym = {"VERIFIED": "[OK]", "CONTRADICTED": "[FAIL]", "UNVERIFIED": "[??]", "INFERRED": "[~~]"}.get(r.status, "?")
        print(f"  {sym} [{r.claim_id}] {r.status}: {r.evidence}")

    # Write output
    output = {
        'summary': {
            'total': total, 'verified': vc, 'contradicted': cc,
            'unverified': uc, 'inferred': ic,
            'accuracy': f"{vc}/{total} ({100*vc//total}%)" if total else "N/A"
        },
        'results': [asdict(r) for r in results],
        'ground_truth_stats': {
            'files': len(gt['files']), 'classes': len(gt['classes']),
            'structs': len(gt['structs']), 'enums': len(gt['enums']),
            'functions': len(gt['functions']), 'includes': len(gt['includes'])
        }
    }

    out_path = os.path.join(args.repo, 'experiments', 'hse009', args.output)
    os.makedirs(os.path.dirname(out_path), exist_ok=True)
    with open(out_path, 'w') as f:
        json.dump(output, f, indent=2)
    print(f"\nResults: {out_path}")


if __name__ == '__main__':
    main()
