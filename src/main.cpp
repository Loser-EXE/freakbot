#include <cstdlib>
#include <dpp/appcommand.h>
#include <dpp/channel.h>
#include <dpp/cluster.h>
#include <dpp/discordevents.h>
#include <dpp/dispatcher.h>
#include <dpp/emoji.h>
#include <dpp/guild.h>
#include <dpp/intents.h>
#include <dpp/message.h>
#include <dpp/once.h>
#include <dpp/permissions.h>
#include <dpp/restresults.h>
#include <dpp/snowflake.h>
#include <dpp/user.h>
#include <functional>
#include <future>
#include <iostream>
#include <string>
#include <map>

const std::string BOT_TOKEN = std::getenv("DISCORD_BOT_TOKEN");
const long KID_NAMED_SERVER_ID = 1277822063985557604;
const long HEART_CHANNEL_ID = 1352420163969880186;

int main() {
        std::map<std::string, std::function<void(const dpp::slashcommand_t&)>> slashCommandMap;

        dpp::cluster bot(BOT_TOKEN);
        bot.intents = dpp::i_default_intents | dpp::i_guild_members;

        bot.on_message_reaction_add([&bot](const dpp::message_reaction_add_t &event) {
                dpp::emoji reactingEmoji = event.reacting_emoji; 

                dpp::snowflake authorId = event.message_author_id;
                dpp::snowflake messageId = event.message_id;
                dpp::snowflake channel_id = event.channel_id;
                dpp::guild &guild = *(event.reacting_guild);


                std::string authorNickname = guild.members.at(authorId).get_nickname();
                std::promise<dpp::message> messagePromise; 
                
                bot.message_get(messageId, channel_id, [&bot, &messagePromise](const dpp::confirmation_callback_t &callback) {
                        if (callback.is_error()) {
                                std::cout << callback.get_error().human_readable;
                        }

                        dpp::message msg = std::get<dpp::message>(callback.value);
                        std::cout << msg.content;
                        messagePromise.set_value(msg);
                });
               
                std::string messageContent = messagePromise.get_future().get().content;
                dpp::message msg = dpp::message(HEART_CHANNEL_ID, messageContent);
                bot.message_create(msg);
        });

        bot.on_ready([&bot](const dpp::ready_t &ready) {
                if (dpp::run_once<struct register_bot_commands>()) {
                        /*bot.guild_bulk_command_delete(KID_NAMED_SERVER_ID);*/
                }
        });

        bot.on_slashcommand([&bot, slashCommandMap](const dpp::slashcommand_t &event) {
                std::string commandName = event.command.get_command_name();
                slashCommandMap.at(commandName)(event);
        });

        bot.on_log(dpp::utility::cout_logger());
	 
	    bot.start(dpp::st_wait);
}
