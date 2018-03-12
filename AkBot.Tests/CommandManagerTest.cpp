#define BOOST_TEST_MODULE CommandManagerTest
#include <boost/test/included/unit_test.hpp>
#include <commands\CommandManager.h>

using namespace AKBot;

class DummyExecutor : public CommandExecutor
{
	std::string lastCommand;
public:
	void onCommand(const std::string& command) override {
		lastCommand = command;
	}
	std::string getLastCommand() {
		return lastCommand;
	}
};
class NoSupportedExecutor : public CommandExecutor
{
public:
	void onCommand(const std::string& command) override {
		BOOST_FAIL(L"Command is not supported");
	}
	bool isSupported(const std::string& command) const override { return false; }
};

BOOST_AUTO_TEST_CASE(CommandPassedToExecutor)
{
	CommandManager commandManager;
	auto executor = std::make_unique<DummyExecutor>();
	auto dummy = executor.get();
	commandManager.registerExecutor(std::move(executor));

	commandManager.execute("Test");
	BOOST_TEST("Test" == dummy->getLastCommand().c_str(), L"Last command should have");
}
BOOST_AUTO_TEST_CASE(NotSupportedCommandsExecuted)
{
	CommandManager commandManager;
	auto executor = std::make_unique<NoSupportedExecutor>();
	auto dummy = executor.get();
	commandManager.registerExecutor(std::move(executor));

	commandManager.execute("Test");
}
BOOST_AUTO_TEST_CASE(FirstSupportedExecutorProcessCommand)
{
	CommandManager commandManager;
	auto firstExecutorPtr = std::make_unique<DummyExecutor>();
	auto firstExecutor = firstExecutorPtr.get();
	commandManager.registerExecutor(std::move(firstExecutorPtr));

	auto secondExecutorPtr = std::make_unique<DummyExecutor>();
	auto secondExecutor = secondExecutorPtr.get();
	commandManager.registerExecutor(std::move(secondExecutorPtr));

	commandManager.execute("Test");
	BOOST_TEST("Test" == firstExecutor->getLastCommand().c_str(), L"First executor should process command");
	BOOST_TEST("" == secondExecutor->getLastCommand().c_str(), L"Second executor should not process command");
}