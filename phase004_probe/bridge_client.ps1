# HSE Bridge Client (PowerShell)
# Sends commands to HSE Live Bridge via named pipe
# Usage:
#   .\bridge_client.ps1                    # interactive
#   .\bridge_client.ps1 -Demo              # run demo
#   .\bridge_client.ps1 -Test              # run test
#   .\bridge_client.ps1 -Command '{"cmd":"health"}'

param(
    [switch]$Demo,
    [switch]$Test,
    [switch]$Health,
    [string]$Command
)

$PipeName = "\\.\pipe\hse_bridge"

function Connect-Bridge {
    while ($true) {
        try {
            $pipe = New-Object System.IO.Pipes.NamedPipeClientStream(".", "hse_bridge", "InOut")
            $pipe.Connect(3000)
            $pipe.ReadMode = [System.IO.Pipes.PipeTransmissionMode]::Byte
            Write-Host "Connected to $PipeName"
            return $pipe
        } catch {
            Write-Host "Waiting for HSE bridge..."
            Start-Sleep -Seconds 1
        }
    }
}

function Send-Command {
    param($pipe, $json)
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($json + "`n")
    $pipe.Write($bytes, 0, $bytes.Length)
    $pipe.Flush()

    $buffer = New-Object byte[] 4096
    $response = ""
    while ($true) {
        $read = $pipe.Read($buffer, 0, 1)
        if ($read -eq 0) { break }
        $char = [char]$buffer[0]
        if ($char -eq "`n") { break }
        if ($char -ne "`r") { $response += $char }
    }
    return $response
}

function Add-Primitive {
    param($pipe, $type="Quad", $px=0, $py=0, $pz=0, $rx=0, $ry=0, $rz=0, $sx=1, $sy=1, $sz=1, $cr=1, $cg=1, $cb=1, $name="")
    $json = @{cmd="add_primitive"; type=$type; px=$px; py=$py; pz=$pz; rx=$rx; ry=$ry; rz=$rz; sx=$sx; sy=$sy; sz=$sz; cr=$cr; cg=$cg; cb=$cb; name=$name} | ConvertTo-Json -Compress
    return Send-Command $pipe $json
}

function Remove-Primitive {
    param($pipe, $index)
    $json = @{cmd="remove_primitive"; index=$index} | ConvertTo-Json -Compress
    return Send-Command $pipe $json
}

function Set-PrimitivePosition {
    param($pipe, $index, $px, $py, $pz)
    $json = @{cmd="set_position"; index=$index; px=$px; py=$py; pz=$pz} | ConvertTo-Json -Compress
    return Send-Command $pipe $json
}

function Set-PrimitiveColor {
    param($pipe, $index, $cr, $cg, $cb)
    $json = @{cmd="set_color"; index=$index; cr=$cr; cg=$cg; cb=$cb} | ConvertTo-Json -Compress
    return Send-Command $pipe $json
}

function Set-Camera {
    param($pipe, $px, $py, $pz, $rx=0, $ry=0, $rz=0, $fov=60)
    $json = @{cmd="set_camera"; px=$px; py=$py; pz=$pz; rx=$rx; ry=$ry; rz=$rz; fov=$fov} | ConvertTo-Json -Compress
    return Send-Command $pipe $json
}

function Get-SceneState {
    param($pipe)
    return Send-Command $pipe '{"cmd":"scene_state"}'
}

function Get-Health {
    param($pipe)
    return Send-Command $pipe '{"cmd":"health"}'
}

$pipe = Connect-Bridge

try {
    if ($Demo) {
        Write-Host "`n=== Demo: Building a colorful room ===" -ForegroundColor Cyan

        Send-Command $pipe '{"cmd":"set_clear_color","r":0.02,"g":0.02,"b":0.05}' | Out-Null
        Set-Camera $pipe 6 4 6 0 0 0 60 | Out-Null

        $objects = @(
            @{type="Quad"; px=0; py=-1.5; pz=0; rx=-90; ry=0; rz=0; sx=8; sy=8; sz=1; cr=0.3; cg=0.25; cb=0.2; name="Floor"},
            @{type="Quad"; px=0; py=1.5; pz=0; rx=90; ry=0; rz=0; sx=8; sy=8; sz=1; cr=0.4; cg=0.35; cb=0.3; name="Ceiling"},
            @{type="Quad"; px=0; py=0; pz=-4; rx=0; ry=0; rz=0; sx=8; sy=3; sz=1; cr=0.5; cg=0.5; cb=0.55; name="Wall B"},
            @{type="Quad"; px=-4; py=0; pz=0; rx=0; ry=90; rz=0; sx=8; sy=3; sz=1; cr=0.55; cg=0.5; cb=0.5; name="Wall L"},
            @{type="Quad"; px=4; py=0; pz=0; rx=0; ry=-90; rz=0; sx=8; sy=3; sz=1; cr=0.55; cg=0.5; cb=0.5; name="Wall R"},
            @{type="Cube"; px=-3; py=-0.5; pz=-3; rx=0; ry=0; rz=0; sx=0.4; sy=2; sz=0.4; cr=0.7; cg=0.3; cb=0.3; name="Pillar 1"},
            @{type="Cube"; px=3; py=-0.5; pz=-3; rx=0; ry=0; rz=0; sx=0.4; sy=2; sz=0.4; cr=0.3; cg=0.7; cb=0.3; name="Pillar 2"},
            @{type="Cube"; px=0; py=-0.75; pz=-1; rx=0; ry=30; rz=0; sx=1; sy=1.5; sz=1; cr=0.3; cg=0.3; cb=0.8; name="Box"}
        )

        foreach ($obj in $objects) {
            Write-Host "Adding $($obj.name)..." -ForegroundColor Yellow
            $resp = Add-Primitive $pipe $obj.type $obj.px $obj.py $obj.pz $obj.rx $obj.ry $obj.rz $obj.sx $obj.sy $obj.sz $obj.cr $obj.cg $obj.cb $obj.name
            Write-Host "  $resp" -ForegroundColor Gray
            Start-Sleep -Milliseconds 50
        }

        Write-Host "`nScene state:" -ForegroundColor Cyan
        $state = Get-SceneState $pipe
        ($state | ConvertFrom-Json | ConvertTo-Json -Depth 5) | Write-Host

    } elseif ($Test) {
        Write-Host "`n=== Test: Create, modify, remove ===" -ForegroundColor Cyan

        Write-Host "Health:" -ForegroundColor Yellow
        Get-Health $pipe | Write-Host

        Write-Host "`n1. Add triangle..." -ForegroundColor Yellow
        $resp = Add-Primitive $pipe "Triangle" 0 0 0 0 0 0 1 1 1 1 0 0
        Write-Host "  $resp" -ForegroundColor Gray
        $idx = ($resp | ConvertFrom-Json).index

        Start-Sleep -Milliseconds 200
        Write-Host "`n2. Set color blue..." -ForegroundColor Yellow
        Set-PrimitiveColor $pipe $idx 0 0.5 1 | Write-Host

        Write-Host "`n3. Set position (2,1,0)..." -ForegroundColor Yellow
        Set-PrimitivePosition $pipe $idx 2 1 0 | Write-Host

        Start-Sleep -Milliseconds 500
        Write-Host "`n4. Remove..." -ForegroundColor Yellow
        Remove-Primitive $pipe $idx | Write-Host

        Write-Host "`n5. Final state:" -ForegroundColor Yellow
        (Get-SceneState $pipe | ConvertFrom-Json | ConvertTo-Json -Depth 5) | Write-Host

    } elseif ($Health) {
        Get-Health $pipe | Write-Host

    } elseif ($Command) {
        $resp = Send-Command $pipe $Command
        ($resp | ConvertFrom-Json | ConvertTo-Json -Depth 5) | Write-Host

    } else {
        Write-Host "`n=== HSE Bridge Interactive Client ===" -ForegroundColor Cyan
        Write-Host "Commands: add, remove, pos, color, camera, state, health, quit" -ForegroundColor Gray
        Write-Host ""

        while ($true) {
            $line = Read-Host "hse>"
            if ($line -eq "quit") { break }
            if (-not $line) { continue }

            $resp = Send-Command $pipe $line
            try {
                ($resp | ConvertFrom-Json | ConvertTo-Json -Depth 5) | Write-Host
            } catch {
                Write-Host $resp
            }
        }
    }
} finally {
    $pipe.Close()
    $pipe.Dispose()
}
