#pragma once

#include <deque>
#include <functional>
#include "user.hpp"
#include "reverse.hpp"
#include "constants.hpp"
#include "types.hpp"
#include "utils.hpp"

/* Example JSON file storing messages to disk
{
	"count": 2,
	"messages":[
		{
			"id":				112,
			"sender": {
				"name": 		"elementary.os",
				"ip_addr": 		[192, 168, 4, 2]		// save to init IPAddress
			},
			"contents": 		"hello world",
			"timestamp": 		"2021-6-8 06:43:21 PM"
		},
		{
			"id":				113,
			"sender": {
				"name":			"manjaro",
				"ip_addr":		[192, 168, 4, 3]
			},
			"contents":			"goodbye world",
			"timestamp":		"2021-6-8 06:43:34 PM"
		}
	]
}
*/

namespace message{

	using user_t = user::user_t;

	struct message_t{
		size_t			id;
		user_t 	sender;
		string_t		contents;
		string_t		timestamp;
	};


	class message_queue{
	public:
		void add_message(const message_t& message);
		void add_message(const user_t& sender, string_t contents, string_t timestamp);
		void add_messages(const message_queue& messages);
		void remove_message(size_t index);
		int get_count() const;
		string_t to_json() const;
		string_t to_json(size_t from) const;
		string_t to_json(size_t from, size_t to) const;
		string_t get_message(size_t index) const;
		bool has_messages() const;
		const message_t& get_message_ref(size_t index) const;
		string_t get_last_message() const;
		std::deque<message_t> get_messages() const;
		void load_json(string_t path);
		void load_json_from_file(string_t json);
		void set_message_status(size_t index, bool status);
		void clear();
		
		static size_t counter;
	private:
		// FIFO queue to push new messages and pop old ones
		std::deque<message_t> m_messages;
	};

}

