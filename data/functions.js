messages = [];
users = [];
let web_socket = new WebSocket(`ws://${window.location.hostname}/web_sockets`);


class User{
	constructor(name, ip = [0, 0, 0, 0]){
		this.name = name;
		this.ip = ip;
	}
}

class Message{
	constructor(sender, contents, timestamp){
		this.sender = sender;
		this.contents = contents;
		this.timestamp = timestamp;
	}
}


function init_client(messages){
	window.addEventListener("mousemove", function(e){
		document.getElementById("brush").style.display = "block";
		document.getElementById("brush").style.left = e.offsetX + "px";
		document.getElementById("brush").style.top = e.offsetY + "px";
	});

	web_socket.onopen = function(e){
		// Send user info to server
		console.log("web_socket.onopen: connection established\n");
	}
	web_socket.onclose = function(e){
		console.log("web_socket.onclose: disconnected from server");
		messages = [];
	}
	web_socket.onmessage = function(e){
		// TODO: Need to check the message type being sent from the server
		var results = parse_json(e.data);
		var data = results[0];
		var type = results[1];
		
		console.log("web_socket.onmessage: \n" + e.data);
		if(type == "message"){
			messages = messages.concat(data);
			document.getElementById("message_list").innerHTML = format_messages(messages);
		}
		else if(type == "user"){
			users = data;
			document.getElementById("user_list").innerHTML = format_users(users);
		}
	}
	web_socket.onerror = function(e){
		console.log("web_socket.onerror: " + e.data);
	}

	// Send request to server saying we're shutting down
	document.addEventListener("visibilitychange", function(e){
		// if(document.visibilityState === "hidden"){
		// 	navigator.sendBeacon("/disconnect");
		// 	web_socket.close();
		// }
	});
}

function get_user_info(){
	var info 		= new Object();
	info.type 		= "user";
	info.name 		= document.getElementById("sender").value;
	return info;
}

function get_request_info(id_user, id_message){
	var sender 		= document.getElementById(id_user).value;
	var contents 	= document.getElementById(id_message).value;
	let c 			= new Date();
	let timestamp 	= (c.getMonth() + 1) 
		+ "-" + c.getDate() 
		+ "-" + c.getFullYear() 
		+ " " + c.getHours() 
		+ ":" + c.getMinutes() 
		+ ":" + c.getSeconds();
		
	var info 		= new Object();
	info.type 		= "message";
	info.message 	= new Message(new User(sender), contents, timestamp);
	return info;
}

function parse_json(data){
	var json = JSON.parse(data);
	var type = json.type;
	var data_list = [];
	switch(type){
		case "user":
			var users = json.users;
			for(var i = 0; i < users.length; i++){
				var data = users[i];
				var user = new User(
					data.name,
					data.ip_addr
				);
				data_list.push(user);
			}
			break;
		case "message":
			var messages = json.messages;
			for(var i = 0; i < messages.length; i++){
				var data = messages[i];
				var sender = new User(
					data.sender.name, 
					data.sender.ip_addr
				)
				var message = new Message(
					sender, 
					data.contents,
					data.timestamp
				);
				data_list.push(message);
			}
			break;
		default:
			console.log("parse_json(): no type specified")
			return [];
	}
	// console.log("parse_json(): \n", data_list);
	return [data_list, type];
}

function textarea_counter(id_label){
	var textarea = document.getElementsByTagName("textarea");
	id_label.value = String(textarea.maxLength - textarea.value) + " characters remaining";
}

function format_messages(messages){
	var html = "<div class=\"row\">\n";
	for(var i = messages.length-1; i >= 0; i--){
		html += format_message(i, messages[i]);
	} 
	html += "</div>\n";
	return html;
}

function format_message(index, message){
	var type = (index % 2 == 0) ? "alt-a" : "alt-b";
	var ip = message.sender.ip;
	var html = "";
	html += "	<div class=\"" + type + "\">\n";
	html += "		<h4>" + message.sender.name+"@"+ip[0]+"."+ip[1]+"."+ip[2]+"."+ip[3];
	html += " (Sent at " + message.timestamp + ")</h4>\n";
	html += "		<p>" + message.contents;
	html += "		</p>\n";
	html += "	</div>\n";
	return html;
}

function format_user(index, user){
	var type = (index % 2 == 0) ? "alt-a" : "alt-b";
	var html = "";
	html += "	<h4 class=\"container " + type + "\">";
	html += user.name+"@"+user.ip[0]+"."+user.ip[1]+"."+user.ip[2]+"."+user.ip[3];
	html += "</h4>";
	return html;
}

function format_users(users){
	var html = "";
	for(var i = users.length-1; i >= 0; i--){
		html += format_user(i, users[i]);
	} 
	return html;
}

function load_messages(messages){
	// Load all messages at once!
	console.log("load_message(): loading messages to web page...");
	var xhttp = new XMLHttpRequest();
	xhttp.open("GET", "load_messages");
	xhttp.onreadystatechange = function(){
		if(this.readyState == 4 && this.status == 200){
			messages = parse_json(this.responseText);
			document.getElementById("message_list").innerHTML = format_messages(messages);
			window.scrollTo(0, document.body.scrollHeight);
			console.log("load_messages(): loaded messages onto web page.");
		}
	}
	xhttp.send();
}

function send_message(id_user, id_message){
	// Send message via web socket as JSON object
	var json = JSON.stringify(get_request_info(id_user, id_message));
	web_socket.send(json);
	console.log("web_socket.send:\n"+json);

	document.getElementById("contents").value = "";
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