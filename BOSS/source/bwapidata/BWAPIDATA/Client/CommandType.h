#pragma once
/**
 *  Used in UnitCommand
 */

namespace BWAPIDATAC
{
  namespace CommandType
  {
    enum Enum
    {
      None,
      SetScreenPosition,
      PingMinimap,
      EnableFlag,
      Printf,
      SendText,
      ChangeRace,
      StartGame,
      PauseGame,
      ResumeGame,
      LeaveGame,
      RestartGame,
      SetLocalSpeed,
      SetTextSize,
      SetLatCom,
      SetGui
    };
  }
}