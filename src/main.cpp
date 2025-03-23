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
                dpp::snowflake channelId = event.channel_id;
                dpp::guild &guild = *(event.reacting_guild);

                dpp::guild_member author = guild.members.at(authorId);

                std::promise<dpp::message> messagePromise; 
                std::promise<dpp::channel> channelPromise;
                
                bot.message_get(messageId, channelId, [&bot, &messagePromise](const dpp::confirmation_callback_t &callback) {
                        if (callback.is_error()) {
                                std::cout << callback.get_error().human_readable;
                        }

                        dpp::message msg = std::get<dpp::message>(callback.value);
                        messagePromise.set_value(msg);
                });

                bot.channel_get(channelId, [&bot, &channelPromise](const dpp::confirmation_callback_t &callback) {
                        if (callback.is_error()) {
                                std::cout << callback.get_error().human_readable;
                        }

                        dpp::channel channel = std::get<dpp::channel>(callback.value);
                        channelPromise.set_value(channel);
                });
               
                dpp::message originalMessage = messagePromise.get_future().get();
                dpp::channel channel = channelPromise.get_future().get();

                int reactionCount = 0;
                for (dpp::reaction reaction : originalMessage.reactions) {
                        if (reaction.emoji_id == reactingEmoji.id) {
                                reactionCount = reaction.count;
                                break;
                        }
                }

                dpp::embed_footer footer;
                footer.set_text("This message was sent on");
                
                dpp::embed embed = dpp::embed()
                        .set_author(author.get_nickname(), "", author.get_avatar_url())
                        .set_title(std::to_string(reactionCount) + " :purple_heart:'s")
                        .set_description(originalMessage.content)
                        .set_timestamp(originalMessage.get_creation_time())
                        .set_footer(footer)
                        .add_field("from", author.get_mention(), true)
                        .add_field("in", channel.get_mention(), true)
                        .set_colour(11177686);

                dpp::message response = dpp::message(HEART_CHANNEL_ID, embed);
                bot.message_create(response);
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
