package objects;

import java.io.Serializable;
import java.util.Vector;


public class Game implements Serializable
{
	private static final long serialVersionUID = -92847343973391774L;

	private Bot 			homebot;
	private Bot 			awaybot;

	private GameStatus 		status;
	private Map 			map;

	private boolean 		hostwon;

	private int 			gameID;

	private long 			time;

	private boolean 		wasDraw;

	private int 			hostScore;
	private int 			awayScore;

	private boolean 		hostcrash;
	private boolean 		awaycrash;

	private Vector<Integer> hostTimers = new Vector<Integer>();
	private Vector<Integer> awayTimers = new Vector<Integer>();

	private boolean 		timeout;
	private int 			finalFrame;

	private int 			round;
	private long			hostTime = 0;
	private long 			guestTime = 0;


	public Game(int iD, int roundt, Bot home, Bot away, Map map) 
	{
		this.round = roundt;
		finalFrame = 0;
		this.map = map;
		this.wasDraw = false;
		this.homebot = home;
		this.awaybot = away;
		hostScore = 0;
		awayScore = 0;
		status = GameStatus.WAITING;
		hostwon = false;
		gameID = iD;
		time = 0;
		this.hostcrash = false;
		this.awaycrash = false;
		this.timeout = false;
	}


	public String getResultString() 
	{
		String s = String
				.format("%7d %5d %20s %20s %35s %6b %6b %6b %6b %8d %8d %8d %10d %10d",
						this.gameID, 
						this.round,
						this.homebot.getName(),
						this.awaybot.getName(),
						this.map.getMapName().replace(' ', '_'), 
						this.hostwon,
						this.hostcrash, 
						this.awaycrash, 
						this.wasDraw,
						this.hostScore,
						this.awayScore, 
						this.finalFrame, 
						(int) this.hostTime,
						(int) this.guestTime);

		for (int i=0; i<hostTimers.size(); ++i)
		{
			String t = String.format(" %5d", hostTimers.get(i));
			s += t;
		}
		
		for (int i=0; i<awayTimers.size(); ++i)
		{
			String t = String.format(" %5d", awayTimers.get(i));
			s += t;
		}
		
		s += System.getProperty("line.separator");
		
		return s;
	}

	
	public GameStatus getStatus() {
		return status;
	}

	public void setStatus(GameStatus status) {
		if (status == GameStatus.DONE) {
			this.time = getTime();
		}
		this.status = status;
	}

	public Bot getHomebot() {
		return homebot;
	}

	
	public Bot getAwaybot() {
		return awaybot;
	}

	public Map getMap() {
		return map;
	}

	public boolean isHostwon() {
		return hostwon;
	}

	public void setHostwon(boolean hostwon) {
		this.hostwon = hostwon;
	}

	public String print(boolean detailed) {
		String ret = "";
		ret = ret
				+ ("(" + gameID + ") -> " + this.status + " -> Map: " + this.map
						.getMapName());
		if (status == GameStatus.DONE) {
			if (hostwon) {
				ret = ret + (" ## " + this.homebot.getName() + " WON");
			} else {
				ret = ret + (" ## " + this.awaybot.getName() + " WON");
			}

			if (detailed) {
				ret = ret
						+ ("\n        Score -> host:" + this.hostScore
								+ " guest:" + this.awayScore);
			}

			if (detailed) {
				if ((this.hostcrash || this.awaycrash)) {
					ret = ret
							+ ("\n        HostCrash: " + this.hostcrash
									+ "  AwayCrash: " + this.awaycrash);
				} else {
					ret = ret + ("\n        Final Frame -> " + this.finalFrame);
				}
			}
			
		}
		ret = ret + "\n";
		return ret;
	}

	public int getGameID() {
		return gameID;
	}

	public long getTime() {
		if (this.status == GameStatus.DONE) {
			return this.time;
		}
		long epoch = System.currentTimeMillis() / 1000;
		return (epoch - this.time);
	}

	public void startTime() {
		this.time = System.currentTimeMillis() / 1000;
	}

	/**
	 * Estimates the time remaining for this Game
	 * 
	 * @return Estimated time in seconds
	 */
	public long estimateTime() {
		int factor = 2;
		if (this.homebot.getName().equals("BuiltIn")
				|| this.awaybot.getName().equals("BuiltIn")) {
			factor = 1;
		}
		if (this.time == 0) {
			return 3600 * factor;
		} else {
			return ((3600 - getTime()) * factor);
		}
	}

	/**
	 * Sets if this game was a draw.
	 * 
	 * @param b
	 *            true if this game was a draw
	 */
	public void setWasDraw(boolean b) {
		this.wasDraw = b;
	}

	/**
	 * @return the wasDraw
	 */
	public boolean isWasDraw() {
		return wasDraw;
	}

	/**
	 * @return the hostScore
	 */
	public int getHostScore() {
		return hostScore;
	}

	/**
	 * @return the awayScore
	 */
	public int getAwayScore() {
		return awayScore;
	}

	/**
	 * Calculates if the host won based on scores. Only does this if the game
	 * was a draw and the tournament module ended it.
	 */
	public void calcResult() {
		if (wasDraw && hostScore != 0 && awayScore != 0) {
			System.out.print("\nGame " + this.gameID + " was a Draw with "
					+ hostScore + " to " + awayScore);
			if (this.hostScore > this.awayScore) {
				this.hostwon = true;
				System.out.println("     Host Wins with " + hostScore + "\n");
			} else {
				this.hostwon = false;
				System.out.println("     Guest Wins with " + awayScore + "\n");
			}
		}
	}

	/**
	 * @param hostScore
	 *            the hostScore to set
	 */
	public void setHostScore(int hostScore) {
		this.hostScore = hostScore;
	}

	/**
	 * @param awayScore
	 *            the awayScore to set
	 */
	public void setAwayScore(int awayScore) {
		this.awayScore = awayScore;
	}

	/**
	 * Getter for the boolean value indicating that the host bot has crashed
	 * 
	 * @return the hostcrash
	 */
	public boolean isHostcrash() {
		return hostcrash;
	}

	/**
	 * Setter for the boolean value indicating that the host bot has crashed
	 * 
	 * @param hostcrash
	 *            the hostcrash to set
	 */
	public void setHostcrash(boolean hostcrash) {
		this.hostcrash = hostcrash;
	}

	/**
	 * Getter for the boolean value indicating that the away bot has crashed
	 * 
	 * @return the awaycrash
	 */
	public boolean isAwaycrash() {
		return awaycrash;
	}

	/**
	 * Setter for the boolean value indicating that the away bot has crashed
	 * 
	 * @param awaycrash
	 *            the awaycrash to set
	 */
	public void setAwaycrash(boolean awaycrash) {
		this.awaycrash = awaycrash;
	}

	public Vector<Integer> getHostTimers() 
	{
		return hostTimers;
	}

	public void setHostTimers(Vector<Integer> timers) 
	{
		if (this.awayTimers.size() == 0)
		{
			for (int i=0; i<timers.size(); ++i)
			{
				awayTimers.add(0);
			}
		}
		
		this.hostTimers = timers;
	}

	public Vector<Integer> getAwayTimers() 
	{
		return awayTimers;
	}

	public void setAwayTimers(Vector<Integer> timers) 
	{
		if (this.hostTimers.size() == 0)
		{
			for (int i=0; i<timers.size(); ++i)
			{
				hostTimers.add(0);
			}
		}
		
		this.awayTimers = timers;
	}
	
	/**
	 * @return the timeout
	 */
	public boolean isTimeout() {
		return timeout;
	}

	/**
	 * @param timeout
	 *            the timeout to set
	 */
	public void setTimeout(boolean timeout) {
		this.timeout = timeout;
	}

	/**
	 * @return the finalFrame
	 */
	public int getFinalFrame() {
		return finalFrame;
	}

	/**
	 * @param finalFrame
	 *            the finalFrame to set
	 */
	public void setFinalFrame(int finalFrame) {
		this.finalFrame = finalFrame;
	}

	/**
	 * @return the round
	 */
	public int getRound() {
		return round;
	}

	/**
	 * @return the hostTime
	 */
	public long getHostTime() {
		return hostTime;
	}

	/**
	 * @param hostTime
	 *            the hostTime to set
	 */
	public void setHostTime(long hostTime) {
		this.hostTime = hostTime;
	}

	/**
	 * @return the guestTime
	 */
	public long getGuestTime() {
		return guestTime;
	}

	/**
	 * @param l
	 *            the guestTime to set
	 */
	public void setGuestTime(long l) {
		this.guestTime = l;
	}

	public void updateWithGame(Game game) {
		if (gameID == game.getGameID()) {
			
			if (game.getHostTimers().size() > 0)
				setHostTimers(game.getHostTimers());
			if (game.getAwayTimers().size() > 0)
				setAwayTimers(game.getAwayTimers());
			setFinalFrame(game.getFinalFrame());
			setWasDraw(game.isWasDraw());
			setStatus(GameStatus.DONE);
			setHostwon(game.isHostwon());
			setTimeout(game.isTimeout());
			if (game.getHostTime() > 0)
				setHostTime(game.getHostTime());
			if (game.getGuestTime() > 0)
				setGuestTime(game.getGuestTime());
			if (game.getHostScore() > 0) {
				setHostScore(game.getHostScore());
			}
			if (game.getAwayScore() > 0) {
				this.setAwayScore(game.getAwayScore());
			}
			if (game.isHostcrash()) {
				if (isAwaycrash()) {
					setHostwon(true);
				} else {
					setHostwon(false);
					setHostcrash(true);
				}
			} else if (game.isAwaycrash()) {
				if (isHostcrash()) {
					setHostwon(false);
				} else {
					setHostwon(true);
					setAwaycrash(true);
				}
			}
			if (game.isTimeout()) {
                calcResult();
            }
		}
	}
}
