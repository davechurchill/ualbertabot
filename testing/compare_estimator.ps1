Param(
    [Parameter(Mandatory)]
	[ValidateNotNullOrEmpty()]
	[ValidateSet('R','T','Z','P')]
	[string]$Race = "R",
	[int]$GamesCount = 0,
	[string]$BotName = $null
)
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

function Measure-Bot($BotName, $GamesCount, $BotsForTesing)
{
	echo "Measure bot $BotName"
	echo "Run agains each bot $GamesCount games"
	echo "
| Bot | Wins | Games | Start | End | Duration | Avg. Duration |
| --- | ---- | ----- | ----- | --- | -------- | ------------- |"
	Foreach ($EnemyBot in $BotsForTesing)
	{
		& $scriptDir\run_games.ps1 -Bot1 $BotName -Bot2 $EnemyBot -Games $GamesCount
	}
}

# These bots work more then 20 minutes.
$StrangeBots = @("OpprimoBot", "Johan Kayser")

$BotsForTesing1 = @("PurpleWave", "100382319", "Hao Pan", "OpprimoBot", "Marine Hell", "Goliat")
$BotsForTesing2 = @("Bjorn P Mattsson", "DaleeTYC", "Oleg Ostroumov", "UC3ManoloBot", "Roman Danielis", "Korean")
$BotsForTesing3 = @("Bryan Weber", "HOLD Z", "Kruecke", "MadMixZ", "AyyyLmao", "Travis Shelton")
$BotsForTesing4 = @("Lukas Moravec", "Jakub Trancik", "MadMixT", "UPStarCraftAI 2016", "KaonBot", "Niels Justesen")

$BotsForTesing = $BotsForTesing1 + $BotsForTesing2 + $BotsForTesing3 + $BotsForTesing4
if ($GamesCount -eq 0)
{
	if ($Race -eq "R")
	{
		$GamesCount = (2*3) * (2*2)
	}
	else
	{
		$GamesCount = (2*2)
	}
}

if ($BotName -eq $null)
{
	Measure-Bot AKBot_Sparcraft:$Race $GamesCount $BotsForTesing
	Measure-Bot AKBot_FAP:$Race $GamesCount $BotsForTesing
}
else
{
	Measure-Bot AKBot_Sparcraft:$Race $GamesCount @($BotName)
	Measure-Bot AKBot_FAP:$Race $GamesCount @($BotName)
}
