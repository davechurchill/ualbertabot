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
	};
}
