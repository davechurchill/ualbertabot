#include "stdafx.h"
#include "CppUnitTest.h"
#include <commands\CommandManager.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AkBotTests
{
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
			Assert::Fail(L"Command is not supported");
		}
		bool isSupported(const std::string& command) const override { return false; }
	};

	TEST_CLASS(CommandManagerTest)
	{
	public:
		TEST_METHOD(CommandPassedToExecutor)
		{
			CommandManager commandManager;
			auto executor = std::make_unique<DummyExecutor>();
			auto dummy = executor.get();
			commandManager.registerExecutor(std::move(executor));

			commandManager.execute("Test");
			Assert::AreEqual("Test", dummy->getLastCommand().c_str(), L"Last command should have");
		}
		TEST_METHOD(NotSupportedCommandsExecuted)
		{
			CommandManager commandManager;
			auto executor = std::make_unique<NoSupportedExecutor>();
			auto dummy = executor.get();
			commandManager.registerExecutor(std::move(executor));

			commandManager.execute("Test");
		}
		TEST_METHOD(FirstSupportedExecutorProcessCommand)
		{
			CommandManager commandManager;
			auto firstExecutorPtr = std::make_unique<DummyExecutor>();
			auto firstExecutor = firstExecutorPtr.get();
			commandManager.registerExecutor(std::move(firstExecutorPtr));

			auto secondExecutorPtr = std::make_unique<DummyExecutor>();
			auto secondExecutor = secondExecutorPtr.get();
			commandManager.registerExecutor(std::move(secondExecutorPtr));

			commandManager.execute("Test");
			Assert::AreEqual("Test", firstExecutor->getLastCommand().c_str(), L"First executor should process command");
			Assert::AreEqual("", secondExecutor->getLastCommand().c_str(), L"Second executor should not process command");
		}
	};
}
