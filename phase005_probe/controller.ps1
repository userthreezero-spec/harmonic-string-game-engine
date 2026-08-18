# HSE-005 Continuous Cognitive Controller
# Performs: OBSERVE → REASON → ACT → OBSERVE → VERIFY → REPEAT
#
# Usage:
#   .\controller.ps1                    # Full demo loop
#   .\controller.ps1 -Quick             # Quick test (3 cycles)
#   .\controller.ps1 -CaptureFrame      # Include frame capture

param(
    [switch]$Quick,
    [switch]$CaptureFrame,
    [int]$MaxCycles = 10
)

$ErrorActionPreference = "Stop"
$PipeName = "hse_bridge"
$Results = @()
$SeqCounter = 0

function Connect-Bridge {
    while ($true) {
        try {
            $pipe = New-Object System.IO.Pipes.NamedPipeClientStream(".", $PipeName, "InOut")
            $pipe.Connect(3000)
            $pipe.ReadMode = [System.IO.Pipes.PipeTransmissionMode]::Byte
            Write-Host "[CONNECTED] Bridge pipe connected" -ForegroundColor Green
            return $pipe
        } catch {
            Write-Host "[WAITING] Bridge not available, retrying..." -ForegroundColor Yellow
            Start-Sleep -Seconds 1
        }
    }
}

function Send-Command {
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
    return $response
}

function Send-CommandParsed {
    param($pipe, $json)
    $resp = Send-Command $pipe $json
    return ($resp | ConvertFrom-Json)
}

function Next-Seq {
    $script:SeqCounter++
    return $script:SeqCounter
}

function Add-Object {
    param($pipe, $type, $name, $px=0, $py=0, $pz=0, $rx=0, $ry=0, $rz=0, $sx=1, $sy=1, $sz=1, $cr=1, $cg=1, $cb=1)
    $seq = Next-Seq
    $json = @{cmd="add_primitive"; seq=$seq; type=$type; name=$name;
              px=$px; py=$py; pz=$pz; rx=$rx; ry=$ry; rz=$rz;
              sx=$sx; sy=$sy; sz=$sz; cr=$cr; cg=$cg; cb=$cb} | ConvertTo-Json -Compress
    return Send-CommandParsed $pipe $json
}

function Set-Position {
    param($pipe, $objectID, $px, $py, $pz)
    $seq = Next-Seq
    $json = @{cmd="set_position"; seq=$seq; object_id=$objectID; px=$px; py=$py; pz=$pz} | ConvertTo-Json -Compress
    return Send-CommandParsed $pipe $json
}

function Set-Rotation {
    param($pipe, $objectID, $rx, $ry, $rz)
    $seq = Next-Seq
    $json = @{cmd="set_rotation"; seq=$seq; object_id=$objectID; rx=$rx; ry=$ry; rz=$rz} | ConvertTo-Json -Compress
    return Send-CommandParsed $pipe $json
}

function Set-Color {
    param($pipe, $objectID, $cr, $cg, $cb)
    $seq = Next-Seq
    $json = @{cmd="set_color"; seq=$seq; object_id=$objectID; cr=$cr; cg=$cg; cb=$cb} | ConvertTo-Json -Compress
    return Send-CommandParsed $pipe $json
}

function Set-Camera {
    param($pipe, $px, $py, $pz, $tx=0, $ty=0, $tz=0, $fov=60)
    $seq = Next-Seq
    $json = @{cmd="set_camera"; seq=$seq; px=$px; py=$py; pz=$pz; rx=$tx; ry=$ty; rz=$tz; fov=$fov} | ConvertTo-Json -Compress
    return Send-CommandParsed $pipe $json
}

function Observe-Scene {
    param($pipe)
    $seq = Next-Seq
    $json = @{cmd="observe_scene"; seq=$seq} | ConvertTo-Json -Compress
    return Send-CommandParsed $pipe $json
}

function Observe-Object {
    param($pipe, $objectID)
    $seq = Next-Seq
    $json = @{cmd="observe_object"; seq=$seq; object_id=$objectID} | ConvertTo-Json -Compress
    return Send-CommandParsed $pipe $json
}

function Observe-Health {
    param($pipe)
    $seq = Next-Seq
    $json = @{cmd="observe_health"; seq=$seq} | ConvertTo-Json -Compress
    return Send-CommandParsed $pipe $json
}

function Observe-Delta {
    param($pipe, $sinceRevision)
    $seq = Next-Seq
    $json = @{cmd="observe_delta"; seq=$seq; since_revision=$sinceRevision} | ConvertTo-Json -Compress
    return Send-CommandParsed $pipe $json
}

function Capture-Frame {
    param($pipe)
    $seq = Next-Seq
    $json = @{cmd="capture_frame"; seq=$seq} | ConvertTo-Json -Compress
    return Send-CommandParsed $pipe $json
}

function Remove-Object {
    param($pipe, $objectID)
    $seq = Next-Seq
    $json = @{cmd="remove_primitive"; seq=$seq; object_id=$objectID} | ConvertTo-Json -Compress
    return Send-CommandParsed $pipe $json
}

function Test-InvalidObject {
    param($pipe)
    Write-Host "`n  [TEST] Invalid object (ID 99999)..." -ForegroundColor DarkGray
    $seq = Next-Seq
    $json = @{cmd="set_position"; seq=$seq; object_id=99999; px=0; py=0; pz=0} | ConvertTo-Json -Compress
    $resp = Send-CommandParsed $pipe $json
    $passed = ($resp.status -eq "failed")
    Write-Host "    Result: $($resp.status) - expected: failed - $(if($passed){'PASS'}else{'FAIL'})" -ForegroundColor $(if($passed){'Green'}else{'Red'})
    return @{test="invalid_object"; passed=$passed; response=$resp}
}

function Test-InvalidCommand {
    param($pipe)
    Write-Host "`n  [TEST] Invalid command..." -ForegroundColor DarkGray
    $seq = Next-Seq
    $json = @{cmd="bogus_command"; seq=$seq} | ConvertTo-Json -Compress
    $resp = Send-CommandParsed $pipe $json
    $passed = ($resp.status -eq "failed")
    Write-Host "    Result: $($resp.status) - expected: failed - $(if($passed){'PASS'}else{'FAIL'})" -ForegroundColor $(if($passed){'Green'}else{'Red'})
    return @{test="invalid_command"; passed=$passed; response=$resp}
}

# ============================================================
# MAIN CONTROLLER LOOP
# ============================================================

Write-Host ""
Write-Host "============================================" -ForegroundColor Cyan
Write-Host "  HSE-005 Cognitive Live Loop Controller" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan
Write-Host ""

$pipe = Connect-Bridge

try {
    # --- PHASE 0: Health check ---
    Write-Host "`n[PHASE 0] Health Check" -ForegroundColor Yellow
    $health = Observe-Health $pipe
    Write-Host "  Renderer: $($health.renderer_running)" -ForegroundColor Gray
    Write-Host "  Bridge connected: $($health.bridge_connected)" -ForegroundColor Gray
    Write-Host "  Scene objects: $($health.scene_objects)" -ForegroundColor Gray
    Write-Host "  Frame count: $($health.frame_count)" -ForegroundColor Gray

    # --- PHASE 1: Build initial scene ---
    Write-Host "`n[PHASE 1] Building initial scene" -ForegroundColor Yellow

    $obj1 = Add-Object $pipe "Quad" "floor_main" 0 -1.5 0 -90 0 0 8 8 1 0.3 0.25 0.2
    $floorID = $obj1.object_id
    Write-Host "  floor_main: id=$floorID, status=$($obj1.status)" -ForegroundColor Gray

    $obj2 = Add-Object $pipe "Quad" "wall_left" -4 0 0 0 90 0 8 3 1 0.6 0.55 0.5
    $wallLID = $obj2.object_id
    Write-Host "  wall_left: id=$wallLID, status=$($obj2.status)" -ForegroundColor Gray

    $obj3 = Add-Object $pipe "Quad" "wall_right" 4 0 0 0 -90 0 8 3 1 0.6 0.55 0.5
    $wallRID = $obj3.object_id
    Write-Host "  wall_right: id=$wallRID, status=$($obj3.status)" -ForegroundColor Gray

    $obj4 = Add-Object $pipe "Quad" "back_wall" 0 0 -4 0 0 0 8 3 1 0.5 0.5 0.55
    $backID = $obj4.object_id
    Write-Host "  back_wall: id=$backID, status=$($obj4.status)" -ForegroundColor Gray

    $obj5 = Add-Object $pipe "Cube" "pillar_1" -3 -0.5 -3 0 0 0 0.4 2 0.4 0.7 0.3 0.3
    $pillar1ID = $obj5.object_id
    Write-Host "  pillar_1: id=$pillar1ID, status=$($obj5.status)" -ForegroundColor Gray

    Set-Camera $pipe 6 4 6 0 0 0 60 | Out-Null
    Write-Host "  Camera set to (6,4,6)" -ForegroundColor Gray

    # --- PHASE 2: Observe initial scene ---
    Write-Host "`n[PHASE 2] Observing initial scene" -ForegroundColor Yellow
    $obs1 = Observe-Scene $pipe
    Write-Host "  Scene revision: $($obs1.scene_revision)" -ForegroundColor Gray
    Write-Host "  Objects: $($obs1.objects.Count)" -ForegroundColor Gray
    Write-Host "  Frame count: $($obs1.frame_count)" -ForegroundColor Gray
    Write-Host "  Timestamp: $($obs1.timestamp)" -ForegroundColor Gray

    $revBefore = $obs1.scene_revision

    # --- PHASE 3: Continuous observe-act-observe loop ---
    Write-Host "`n[PHASE 3] Continuous cognitive loop" -ForegroundColor Yellow

    $cycleActions = @(
        @{desc="Move wall_left to x=-5"; cmd={ param($p) Set-Position $p $wallLID -5 0 0 }; verify={ param($o) $o.objects | Where-Object { $_.id -eq $wallLID } | ForEach-Object { [math]::Abs($_.position[0] - (-5)) -lt 0.01 }}},
        @{desc="Rotate wall_right 45deg Y"; cmd={ param($p) Set-Rotation $p $wallRID 0 45 0 }; verify={ param($o) $o.objects | Where-Object { $_.id -eq $wallRID } | ForEach-Object { [math]::Abs($_.rotation[1] - 45) -lt 0.01 }}},
        @{desc="Color pillar_1 blue"; cmd={ param($p) Set-Color $p $pillar1ID 0.2 0.4 0.9 }; verify={ param($o) $o.objects | Where-Object { $_.id -eq $pillar1ID } | ForEach-Object { [math]::Abs($_.color[0] - 0.2) -lt 0.01 }}},
        @{desc="Move camera to (-5,3,5)"; cmd={ param($p) Set-Camera $p -5 3 5 0 0 0 50 }; verify={ param($o) [math]::Abs($o.camera.position[0] - (-5)) -lt 0.01 }},
        @{desc="Move wall_left back to x=-4"; cmd={ param($p) Set-Position $p $wallLID -4 0 0 }; verify={ param($o) $o.objects | Where-Object { $_.id -eq $wallLID } | ForEach-Object { [math]::Abs($_.position[0] - (-4)) -lt 0.01 }}},
        @{desc="Scale pillar_1 to 1.5x"; cmd={ param($p) $seq = Next-Seq; Send-CommandParsed $p (@{cmd="set_scale"; seq=$seq; object_id=$pillar1ID; sx=1.5; sy=1.5; sz=1.5} | ConvertTo-Json -Compress) }; verify={ param($o) $o.objects | Where-Object { $_.id -eq $pillar1ID } | ForEach-Object { [math]::Abs($_.scale[0] - 1.5) -lt 0.01 }}},
        @{desc="Color floor green"; cmd={ param($p) Set-Color $p $floorID 0.2 0.7 0.3 }; verify={ param($o) $o.objects | Where-Object { $_.id -eq $floorID } | ForEach-Object { [math]::Abs($_.color[1] - 0.7) -lt 0.01 }}},
        @{desc="Move camera back to (6,4,6)"; cmd={ param($p) Set-Camera $p 6 4 6 0 0 0 60 }; verify={ param($o) [math]::Abs($o.camera.position[0] - 6) -lt 0.01 }}
    )

    $cycles = if ($Quick) { [math]::Min(3, $cycleActions.Count) } else { [math]::Min($MaxCycles, $cycleActions.Count) }

    for ($i = 0; $i -lt $cycles; $i++) {
        $action = $cycleActions[$i]
        Write-Host "`n  --- Cycle $($i+1)/$($cycles): $($action.desc) ---" -ForegroundColor Cyan

        # ACT
        Write-Host "  [ACT] Executing command..." -ForegroundColor DarkYellow
        $actResult = & $action.cmd $pipe
        Write-Host "    Ack: seq=$($actResult.seq), status=$($actResult.status)" -ForegroundColor Gray
        if ($actResult.status -ne "accepted") {
            Write-Host "    ERROR: Command not accepted!" -ForegroundColor Red
            $Results += @{cycle=$i+1; desc=$action.desc; ack=$actResult; verified=$false; error="not_accepted"}
            continue
        }

        Start-Sleep -Milliseconds 100

        # OBSERVE
        Write-Host "  [OBSERVE] Querying scene..." -ForegroundColor DarkYellow
        $obs = Observe-Scene $pipe
        Write-Host "    Revision: $($obs.scene_revision), Objects: $($obs.objects.Count)" -ForegroundColor Gray

        # VERIFY
        $verified = & $action.verify $obs
        $status = if ($verified) { "CONFIRMED" } else { "UNVERIFIED" }
        $color = if ($verified) { "Green" } else { "Red" }
        Write-Host "  [VERIFY] $status" -ForegroundColor $color

        $Results += @{cycle=$i+1; desc=$action.desc; ack=$actResult; verified=$verified; revision=$obs.scene_revision}

        if ($CaptureFrame -and $i % 3 -eq 0) {
            Write-Host "  [CAPTURE] Getting frame..." -ForegroundColor DarkYellow
            $frame = Capture-Frame $pipe
            if ($frame.status -ne "failed") {
                Write-Host "    Frame: $($frame.width)x$($frame.height), data length: $($frame.data.Length)" -ForegroundColor Gray
            }
        }
    }

    # --- PHASE 4: Delta observation ---
    Write-Host "`n[PHASE 4] Delta observation" -ForegroundColor Yellow
    $delta = Observe-Delta $pipe $revBefore
    Write-Host "  From revision: $($delta.from_revision)" -ForegroundColor Gray
    Write-Host "  To revision: $($delta.to_revision)" -ForegroundColor Gray
    Write-Host "  Changes: $($delta.change_count)" -ForegroundColor Gray
    foreach ($change in $delta.changes) {
        Write-Host "    $($change.object_id).$($change.property): $($change.old) -> $($change.new)" -ForegroundColor DarkGray
    }

    # --- PHASE 5: Failure condition tests ---
    Write-Host "`n[PHASE 5] Failure condition tests" -ForegroundColor Yellow
    $Results += (Test-InvalidObject $pipe)
    $Results += (Test-InvalidCommand $pipe)

    # --- PHASE 6: Final observation ---
    Write-Host "`n[PHASE 6] Final observation" -ForegroundColor Yellow
    $final = Observe-Scene $pipe
    Write-Host "  Scene revision: $($final.scene_revision)" -ForegroundColor Gray
    Write-Host "  Objects:" -ForegroundColor Gray
    foreach ($obj in $final.objects) {
        Write-Host "    $($obj.name) [id=$($obj.id)] pos=$($obj.position) color=$($obj.color)" -ForegroundColor DarkGray
    }

    # --- Summary ---
    Write-Host "`n============================================" -ForegroundColor Cyan
    Write-Host "  EXPERIMENT SUMMARY" -ForegroundColor Cyan
    Write-Host "============================================" -ForegroundColor Cyan
    $totalTests = $Results.Count
    $passedTests = ($Results | Where-Object { $_.passed -eq $true -or $_.verified -eq $true }).Count
    Write-Host "  Total cycles: $cycles" -ForegroundColor White
    Write-Host "  Total tests: $totalTests" -ForegroundColor White
    Write-Host "  Passed: $passedTests / $totalTests" -ForegroundColor $(if($passedTests -eq $totalTests){'Green'}else{'Yellow'})
    Write-Host "  Final revision: $($final.scene_revision)" -ForegroundColor White
    Write-Host "  Final objects: $($final.objects.Count)" -ForegroundColor White

    # Write results JSON
    $resultsObj = @{
        experiment = "HSE-005"
        timestamp = (Get-Date -Format "yyyy-MM-ddTHH:mm:ss")
        total_cycles = $cycles
        total_tests = $totalTests
        passed_tests = $passedTests
        final_revision = $final.scene_revision
        final_object_count = $final.objects.Count
        results = $Results | ForEach-Object {
            @{
                cycle = $_.cycle
                description = $_.desc
                ack_status = $_.ack.status
                verified = $_.verified
                revision = $_.revision
            }
        }
    }
    $resultsJson = $resultsObj | ConvertTo-Json -Depth 5
    $resultsPath = "C:\Users\Rico\HarmonicStringEngine\experiments\hse005\PHASE_HSE005_RESULTS.json"
    $resultsJson | Out-File -FilePath $resultsPath -Encoding UTF8
    Write-Host "`n  Results written to: $resultsPath" -ForegroundColor Gray

} finally {
    $pipe.Close()
    $pipe.Dispose()
    Write-Host "`n[DISCONNECTED] Bridge pipe closed" -ForegroundColor Yellow
}
