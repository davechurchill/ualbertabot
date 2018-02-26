Param(
    [string]$Bot1,
    [string]$Bot2,
	[int]$Games,
    [string]$Map = "sscai/(2)Benzene.scx"
)

$wins = 0
$startDate = Get-Date
For ($i=0; $i -lt $Games; $i++)
{
	scbw.play --bots "$Bot1" "$Bot2" --map "$Map" --headless --timeout 1200 --game_speed 0 2>&1 >output.txt
    $gameResult = Get-Content output.txt
	$winnerCode = $gameResult | Select-Object -Last 1
	if ($winnerCode -eq "0")
	{
		$wins = $wins + 1
	}
}

$finishDate = Get-Date
$duration = ($finishDate - $startDate).TotalSeconds
$averageDuration = $duration / $Games
echo "| $Bot2 | $wins | $Games | $startDate | $finishDate | $duration | $averageDuration |"
#echo "Wins of $Bot1 agains $Bot2 => $wins out of $Games games"
