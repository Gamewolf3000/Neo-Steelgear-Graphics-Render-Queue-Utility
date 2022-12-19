$libs = "Neo Steelgear Graphics Render Queue Utility64D.lib", "Neo Steelgear Graphics Render Queue Utility64.lib"

Get-ChildItem -Path "NSGG Render Queue Utility" -Include *.* -File -Recurse | foreach { $_.Delete()}

foreach ($file in $libs)
{
	

	$path = Get-ChildItem -Path "" -Filter $file -Recurse -ErrorAction SilentlyContinue -Force
	if ($path)
	{
		Copy-Item $path.FullName -Destination "NSGG Render Queue Utility\\Libraries\\"
	}
	else
	{
		Write-Host "Could not find file:" $file
	}
}

Get-ChildItem -Path ".\Neo Steelgear Graphics Render Queue Utility" -Filter *.h* | foreach { Copy-Item $_.FullName -Destination "NSGG Render Queue Utility\\Headers\\"}