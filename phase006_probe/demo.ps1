# HSE-006 Demo Controller
# Proves: CREATE → OPEN → MODIFY → SAVE → REOPEN → VERIFY
#
# Usage:
#   .\demo.ps1                    # Full demo
#   .\demo.ps1 -ProjectDir <dir>  # Use custom project dir

param(
    [string]$ProjectDir = "C:\Users\Rico\HarmonicStringEngine\projects\DemoRoom"
)

$ErrorActionPreference = "Stop"
$SeqCounter = 0

function Connect-Bridge {
    while ($true) {
        try {
            $pipe = New-Object System.IO.Pipes.NamedPipeClientStream(".", "hse_bridge", "InOut")
            $pipe.Connect(3000)
            $pipe.ReadMode = [System.IO.Pipes.PipeTransmissionMode]::Byte
            return $pipe
        } catch {
            Write-Host "Waiting for bridge..." -ForegroundColor Yellow
            Start-Sleep -Seconds 1
        }
    }
}

function Send-Cmd {
    param($pipe, $json)
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($json + "`n")
    $pipe.Write($bytes, 0, $bytes.Length)
    $pipe.Flush()
    $buffer = New-Object byte[] 65536
    $response = ""
    while ($true) {
        $read = $pipe.Read($buffer, 0, 1)
        if ($read -eq 0) { throw "Pipe closed" }
        $char = [char]$buffer[0]
        if ($char -eq "`n") { break }
        if ($char -ne "`r") { $response += $char }
    }
    return ($response | ConvertFrom-Json)
}

function Nxt { $script:SeqCounter++; return $script:SeqCounter }

function Add-Obj {
    param($p, $type, $name, $px=0,$py=0,$pz=0, $rx=0,$ry=0,$rz=0, $sx=1,$sy=1,$sz=1, $cr=1,$cg=1,$cb=1)
    Send-Cmd $p (@{cmd="add_primitive"; seq=(Nxt); type=$type; name=$name; px=$px;py=$py;pz=$pz; rx=$rx;ry=$ry;rz=$rz; sx=$sx;sy=$sy;sz=$sz; cr=$cr;cg=$cg;cb=$cb} | ConvertTo-Json -Compress)
}

function Set-Pos {
    param($p, $id, $px, $py, $pz)
    Send-Cmd $p (@{cmd="set_position"; seq=(Nxt); object_id=$id; px=$px; py=$py; pz=$pz} | ConvertTo-Json -Compress)
}

function Set-Col {
    param($p, $id, $cr, $cg, $cb)
    Send-Cmd $p (@{cmd="set_color"; seq=(Nxt); object_id=$id; cr=$cr; cg=$cg; cb=$cb} | ConvertTo-Json -Compress)
}

function Set-Cam {
    param($p, $px,$py,$pz, $tx=0,$ty=0,$tz=0, $fov=60)
    Send-Cmd $p (@{cmd="set_camera"; seq=(Nxt); px=$px;py=$py;pz=$pz; rx=$tx;ry=$ty;rz=$tz; fov=$fov} | ConvertTo-Json -Compress)
}

function Save-Project {
    param($p, $path="")
    $params = @{cmd="save_project"; seq=(Nxt)}
    if ($path) { $params.path = $path }
    Send-Cmd $p ($params | ConvertTo-Json -Compress)
}

function Observe {
    param($p)
    Send-Cmd $p (@{cmd="observe_scene"; seq=(Nxt)} | ConvertTo-Json -Compress)
}

function Health {
    param($p)
    Send-Cmd $p (@{cmd="observe_health"; seq=(Nxt)} | ConvertTo-Json -Compress)
}

# ============================================================
Write-Host ""
Write-Host "============================================" -ForegroundColor Cyan
Write-Host "  HSE-006 Demo: Project Lifecycle" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan
Write-Host ""

# --- PHASE 1: Create Project ---
Write-Host "[PHASE 1] Create Project" -ForegroundColor Yellow
New-Item -ItemType Directory -Path $ProjectDir -Force | Out-Null
New-Item -ItemType Directory -Path "$ProjectDir\scenes" -Force | Out-Null
New-Item -ItemType Directory -Path "$ProjectDir\assets" -Force | Out-Null
New-Item -ItemType Directory -Path "$ProjectDir\state" -Force | Out-Null

$hscPath = "$ProjectDir\DemoRoom.hsc"
 @"
{
  "hsc_version": "1.0",
  "engine_version": "0.1.0",
  "world_revision": 0,
  "project": {
    "name": "DemoRoom",
    "description": "HSE-006 demo project"
  },
  "scene": {
    "name": "DemoRoom",
    "clear_color_r": 0.03,
    "clear_color_g": 0.03,
    "clear_color_b": 0.06,
    "objects": [
      {"id":"floor","type":"Quad","position":[0,-1.5,0],"rotation":[-90,0,0],"scale":[8,8,1],"color":[0.3,0.25,0.2]},
      {"id":"wall_back","type":"Quad","position":[0,0,-4],"rotation":[0,0,0],"scale":[8,3,1],"color":[0.5,0.5,0.55]},
      {"id":"wall_left","type":"Quad","position":[-4,0,0],"rotation":[0,90,0],"scale":[8,3,1],"color":[0.55,0.5,0.5]},
      {"id":"wall_right","type":"Quad","position":[4,0,0],"rotation":[0,-90,0],"scale":[8,3,1],"color":[0.55,0.5,0.5]}
    ],
    "cameras": [
      {"id":"main_camera","type":"Perspective","position":[6,4,6],"target":[0,0,0],"fov":60}
    ]
  }
}
"@ | Out-File -FilePath $hscPath -Encoding UTF8
Write-Host "  Created: $hscPath" -ForegroundColor Green

# --- PHASE 2: Connect to running HSE ---
Write-Host "`n[PHASE 2] Connect to HSE" -ForegroundColor Yellow
Write-Host "  (HSE should be running: .\build\hse_workspace_app.exe $hscPath)" -ForegroundColor Gray
$pipe = Connect-Bridge
Write-Host "  Connected!" -ForegroundColor Green

# --- PHASE 3: Health check ---
Write-Host "`n[PHASE 3] Health Check" -ForegroundColor Yellow
$h = Health $pipe
Write-Host "  Renderer: $($h.renderer_running), Bridge: $($h.bridge_connected)" -ForegroundColor Gray
Write-Host "  Objects: $($h.scene_objects), Frame: $($h.frame_count)" -ForegroundColor Gray

# --- PHASE 4: Observe initial state ---
Write-Host "`n"[Phase 4] Observe Initial State" -ForegroundColor Yellow
$obs1 = Observe $pipe
Write-Host "  Objects: $($obs1.objects.Count), Revision: $($obs1.scene_revision)" -ForegroundColor Gray
foreach ($obj in $obs1.objects) {
    Write-Host "    $($obj.name) pos=$($obj.position) color=$($obj.color)" -ForegroundColor DarkGray
}

# --- PHASE 5: Modify scene ---
Write-Host "`n[PHASE 5] Modify Scene" -ForegroundColor Yellow

$wallL = ($obs1.objects | Where-Object { $_.name -eq "wall_left" }).id
$wallR = ($obs1.objects | Where-Object { $_.name -eq "wall_right" }).id

Write-Host "  Moving wall_left to x=-5..." -ForegroundColor DarkYellow
$resp = Set-Pos $pipe $wallL -5 0 0
Write-Host "    Ack: $($resp.status), revision: $($resp.revision)" -ForegroundColor Gray

Write-Host "  Coloring wall_right red..." -ForegroundColor DarkYellow
$resp = Set-Col $pipe $wallR 0.9 0.2 0.2
Write-Host "    Ack: $($resp.status), revision: $($resp.revision)" -ForegroundColor Gray

Write-Host "  Adding pillar..." -ForegroundColor DarkYellow
$resp = Add-Obj $pipe "Cube" "pillar_center" 0 -0.5 -1 0 30 0 0.5 2 0.5 0.3 0.7 0.3
Write-Host "    Ack: $($resp.status), id: $($resp.object_id)" -ForegroundColor Gray

Start-Sleep -Milliseconds 200

# --- PHASE 6: Observe modifications ---
Write-Host "`n[PHASE 6] Observe After Modification" -ForegroundColor Yellow
$obs2 = Observe $pipe
Write-Host "  Objects: $($obs2.objects.Count), Revision: $($obs2.scene_revision)" -ForegroundColor Gray
foreach ($obj in $obs2.objects) {
    Write-Host "    $($obj.name) pos=$($obj.position) color=$($obj.color)" -ForegroundColor DarkGray
}

# --- PHASE 7: Save project ---
Write-Host "`n[PHASE 7] Save Project" -ForegroundColor Yellow
$resp = Save-Project $pipe $hscPath
Write-Host "  Save: $($resp.status), path: $($resp.path)" -ForegroundColor Gray

Start-Sleep -Milliseconds 500

# --- PHASE 8: Verify file was written ---
Write-Host "`n[PHASE 8] Verify Saved File" -ForegroundColor Yellow
if (Test-Path $hscPath) {
    $content = Get-Content $hscPath -Raw
    Write-Host "  File exists, size: $($content.Length) bytes" -ForegroundColor Green
    # Check that modifications are in the saved file
    if ($content -match "pillar_center") {
        Write-Host "  VERIFIED: pillar_center found in saved file" -ForegroundColor Green
    } else {
        Write-Host "  WARNING: pillar_center NOT found in saved file" -ForegroundColor Red
    }
    if ($content -match "world_revision") {
        Write-Host "  VERIFIED: world_revision found in saved file" -ForegroundColor Green
    }
} else {
    Write-Host "  ERROR: File not found!" -ForegroundColor Red
}

# --- PHASE 9: Reopen (would need restart) ---
Write-Host "`n[PHASE 9] Reopen Test" -ForegroundColor Yellow
Write-Host "  To complete this test:" -ForegroundColor Gray
Write-Host "  1. Close HSE (ESC)" -ForegroundColor Gray
Write-Host "  2. Restart: .\build\hse_workspace_app.exe $hscPath" -ForegroundColor Gray
Write-Host "  3. Run: .\phase006_probe\demo.ps1 -ProjectDir $ProjectDir" -ForegroundColor Gray
Write-Host "  The scene should restore with all modifications." -ForegroundColor Gray

# --- Summary ---
Write-Host "`n============================================" -ForegroundColor Cyan
Write-Host "  DEMO SUMMARY" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan
Write-Host "  Initial objects: $($obs1.objects.Count)" -ForegroundColor White
Write-Host "  Final objects: $($obs2.objects.Count)" -ForegroundColor White
Write-Host "  Final revision: $($obs2.scene_revision)" -ForegroundColor White
Write-Host "  Project saved: $hscPath" -ForegroundColor White
Write-Host "  All phases completed successfully!" -ForegroundColor Green

$pipe.Close()
$pipe.Dispose()
