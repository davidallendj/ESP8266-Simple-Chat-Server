messages = []
class Message{
	constructor(name, ip, contents, timestamp){
		this.name = name;
		this.ip = ip;
		this.contents = contents;
		this.timestamp = timestamp;
	}
}

function init_client(messages){
	if(!!window.EventSource){
		var source = new EventSource("/web_events");

		source.addEventListener("open", function(e){
			console.log("init_client(): connected to server.");
		}, false);

		source.addEventListener("error", function(e){
			if(e.target.readyState != EventSource.OPEN){
				console.log("init_client(): disconnected from server");
			}
		}, false);
		// Receives "update_messages" payload from server
		source.addEventListener("update_message", function(e){
			messages = messages.concat(parse_messages(e.data));
			console.log("init_client().messages: ", messages);
			document.getElementById("message_list").innerHTML = format_messages(messages);
		});

	}

	// Send request to server saying we're shutting down
	document.addEventListener("visibilitychange", function(e){
		if(document.visibilityState === "hidden"){
			navigator.sendBeacon('/disconnect');
		}
	})
}

function parse_messages(data){
	var json = JSON.parse(data);
	var messages = json.messages;
	var message_list = [];
	for(var i = 0; i < messages.length; i++){
		var data = messages[i];
		var message = new Message(
			data.sender.name, 
			data.sender.ip_addr,
			data.contents,
			data.timestamp
		);
		message_list.push(message);
	}
	console.log("parse_messages().message_list: \n", message_list);
	return message_list;
}

function textarea_counter(id_label){
	var textarea = document.getElementsByTagName("textarea");
	id_label.value = String(textarea.maxLength - textarea.value) + " characters remaining";
}

function format_messages(messages){
	var html = "<div class=\"row\">\n";
	for(var i = messages.length-1; i >= 0; i--){
		html += format_message(i, messages[i])
	}
	html += "</div>\n";
	return html;
}

function format_message(index, message){
	console.log("format_message().message: ", message);
	var color = (index % 2 == 0) ? "#aaa" : "#ccc";
	var html = "";
	html += "	<div style=\"background-color:"+color+"; padding:8px;\">\n";
	html += "		<h4>" + message.name+ "@" + message.ip[0]+"."+ message.ip[1]+"."+ message.ip[2]+"."+message.ip[3];
	html += " (Sent at " + message.timestamp + ")</h4>\n";
	html += "		<p>" + message.contents;
	html += "		</p>\n";
	html += "	</div>\n";
	return html;
}

function load_messages(messages){
	// Load all messages at once!
	console.log("load_message(): loading messages to web page...");
	var xhttp = new XMLHttpRequest();
	xhttp.open("GET", "load_messages");
	xhttp.onreadystatechange = function(){
		if(this.readyState == 4 && this.status == 200){
			messages = parse_messages(this.responseText);
			document.getElementById("message_list").innerHTML = format_messages(messages);
			window.scrollTo(0, document.body.scrollHeight);
			console.log("load_messages(): loaded messages onto web page.");
		}
	}
	xhttp.send();
}

function send_message(id_user, id_message){
	// Send message to server and update local client
	console.log("send_message(): loading message data from server...");

	var user 		= document.getElementById(id_user).value;
	var message 	= document.getElementById(id_message).value;
	var timestamp 	= new Date();
	let c = new Date()
	let f = c.getFullYear() + "-" + (c.getMonth() + 1) + "-" + c.getDate() + " " + c.getHours() + ":" + c.getMinutes() + ":" + c.getSeconds() 
	
	var xhttp = new XMLHttpRequest();
	xhttp.open("POST", "send_message");
	xhttp.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
	xhttp.onreadystatechange = function(){
		if(this.readyState == 4 && this.status == 200){
		}
	};
	xhttp.send(encodeURI("user="+user+"&message="+message+"&timestamp="+f));
	document.getElementById("message").value = "";
};

function play_sound(){
	var context = new AudioContext();
	var o = context.createOscillator();
	o.type = "triangle";
	var g = context.createGain();
	var x = 1.50;
	o.connect(g);
	g.connect(context.destination);
	o.frequency.value = 440.0;
	o.start(0);
	g.gain.exponentialRampToValueAtTime(
		0.00001, context.currentTime + x
	);
}