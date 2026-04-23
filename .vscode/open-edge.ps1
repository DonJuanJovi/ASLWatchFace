param(
    [Parameter(Mandatory=$true)]
    [string]$Url
)

$edgeX86 = 'C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe'
$edge = 'C:\Program Files\Microsoft\Edge\Application\msedge.exe'

if (Test-Path $edgeX86) {
    Start-Process -FilePath $edgeX86 -ArgumentList @('--new-window', $Url)
    exit 0
}

if (Test-Path $edge) {
    Start-Process -FilePath $edge -ArgumentList @('--new-window', $Url)
    exit 0
}

exit 1
