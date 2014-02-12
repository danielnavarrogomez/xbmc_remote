//    By daniel.navarro.gomez@gmail.com
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//    <http://www.gnu.org/licenses/>.


var window;
var server="";
var server_url="";
var user="";
var pass="";
var active_player=1;
var http_timeout=1200;
var actual_vol=0;


function send_json_to_xbmc(json_string){
	get_active_player();
	var req = new XMLHttpRequest();
	req.open('GET', server_url+encodeURI(json_string), true);
	req.timeout = http_timeout;
	req.onload = function(e) {
		if (req.readyState == 4 && req.status == 200) {
			if(req.status == 200) {
				//console.log("Button sent.");
			}
			else { 
				console.log("Error1");
				Pebble.showSimpleNotificationOnPebble("ERROR", "Unable to connect:\n "+server_url);
			}
		}
		else{
			console.log("Error2");
			Pebble.showSimpleNotificationOnPebble("ERROR", "Unable to connect:\n "+server_url);
		}
	};
	req.onerror = function(e) {
		console.log("Error3");
		Pebble.showSimpleNotificationOnPebble("ERROR", "Unable to connect:\n "+server_url);
	};
	req.onabort = function(e) {
		console.log("Error4");
		Pebble.showSimpleNotificationOnPebble("ERROR", "Unable to connect:\n "+server_url);
	};
	req.send(null);
}

function get_active_player(){	
	var req = new XMLHttpRequest();
	req.open('GET', server_url+encodeURI('/jsonrpc?request={"jsonrpc":"2.0","method":"Player.GetActivePlayers","id":1}'), true);
	req.timeout = http_timeout;
	req.onload = function(e) {
		if (req.readyState == 4 && req.status == 200) {
			if(req.status == 200) {
				//console.log();
				var response = JSON.parse(req.responseText);
				if (response) {
					var playerResult = response.result[0];
					active_player = playerResult.playerid;
					//console.log("ActivePlayer: "+active_player);
					window.localStorage.setItem(4, active_player);
				}
			}
		}
	};
	req.send(null);
}


function get_player_info(){	
	var req = new XMLHttpRequest();
	req.open('GET', server_url+encodeURI('/jsonrpc?request={"jsonrpc":"2.0","method":"Player.GetItem","params":{"properties":["title","album","artist","duration","thumbnail","streamdetails"],"playerid":'+active_player+'},"id":"GetItem"}'), true);
	req.timeout = http_timeout;
	req.onload = function(e) {
		if (req.readyState == 4 && req.status == 200) {
			if(req.status == 200) {
				//console.log();
				var response = JSON.parse(req.responseText);
				if (response) {
					
					if (typeof response.result === 'undefined') {
						//console.log("RESULT: No Data");
						Pebble.sendAppMessage({0:"No Data"});
					}
					else{
						//console.log("File: "+response.result.item.label);
						//console.log("Title: "+response.result.item.title);
						if (response.result.item.title!==""){
							//console.log("RESULT: "+response.result.item.title);
							Pebble.sendAppMessage({0:response.result.item.title.substring(0,55)});
						}
						else{
							//console.log("RESULT: "+response.result.item.label);
							Pebble.sendAppMessage({0:response.result.item.label.substring(0,55)});
						}
					}
				}
			}
		}
	};
	req.send(null);
	var req2 = new XMLHttpRequest();
	req2.open('GET', server_url+encodeURI('/jsonrpc?request={"jsonrpc":"2.0","method":"Player.GetProperties","params":{"playerid":'+active_player+',"properties":["percentage"]},"id":"1"}'), true);
	req2.timeout = http_timeout;
	req2.onload = function(e) {
		if (req2.readyState == 4 && req2.status == 200) {
			if(req2.status == 200) {
				//console.log();
				var response2 = JSON.parse(req2.responseText);
				if (response2) {
					
					if (typeof response2.result === 'undefined') {
						//console.log("% done: No Data");
						Pebble.sendAppMessage({1:0});
					}
					else{
						//console.log("% done: "+parseInt(response2.result.percentage));
						Pebble.sendAppMessage({1:parseInt(response2.result.percentage)});
					}
				}
			}
		}
	};
	req2.send(null);
}


function get_vol(){	
	var req = new XMLHttpRequest();
	req.open('GET', server_url+encodeURI('/jsonrpc?request={"jsonrpc":"2.0","method":"Application.GetProperties","params":{"properties":["volume"]},"id": 1}'), true);
	req.timeout = http_timeout;
	req.onload = function(e) {
		if (req.readyState == 4 && req.status == 200) {
			if(req.status == 200) {
				//console.log();
				var response = JSON.parse(req.responseText);
				if (response) {
					//console.log("Actula Vol: "+response.result.volume);
					actual_vol=response.result.volume;
				}
			}
		}
	};
	req.send(null);
}

function button_up(){	
	send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Input.Up","params":[]}');
}

function button_down(){	
	send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Input.Down","params":[]}');
}

function button_left(){	
	send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Input.Left","params":[]}');
}

function button_right(){	
	send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Input.Right","params":[]}');
}

function button_select(){	
	send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Input.Select","params":[]}');
}

function button_back(){	
	send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Input.Back","params":[]}');
}

function button_home(){	
	send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Input.Home","params":[]}');
}

function button_title(){	
	send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Input.ContextMenu","params":[]}');
}

function button_info(){	
	send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Input.Info","params":[]}');
}

function button_action_rev(){	
	//send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Player.SetSpeed","params":{"playerid":'+active_player+',"speed":-32},"id":1}');
	send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Player.Seek","params":{"playerid":'+active_player+',"value":"bigbackward"},"id":1}');
}

function button_action_ff(){
	//send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Player.SetSpeed","params":{"playerid":'+active_player+',"speed":32},"id":1}');
	send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Player.Seek","params":{"playerid":'+active_player+',"value":"bigforward"},"id":1}');
}

function button_action_long_rev(){	
	//send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Player.SetSpeed","params":{"playerid":'+active_player+',"speed":-32},"id":1}');
	send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Player.GoTo","params":{"playerid":'+active_player+',"to":"previous"},"id":1}');
}

function button_action_long_ff(){	
	//send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Player.SetSpeed","params":{"playerid":'+active_player+',"speed":32},"id":1}');
	send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Player.GoTo","params":{"playerid":'+active_player+',"to":"next"},"id":1}');
}


function button_action_stop(){	
	send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Player.Stop","params":{"playerid":'+active_player+'},"id":1}');
}

function button_action_select(){	
	send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Player.PlayPause","params":{"playerid":'+active_player+'},"id":1}');
}

function button_vol_up(){
	get_vol();
	actual_vol=actual_vol+10;
	if (actual_vol>100){
		actual_vol=100;
	}
	send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Application.SetVolume","params":{"volume":'+actual_vol+'},"id":1}');
}

function button_vol_down(){	
	get_vol();
	actual_vol=actual_vol-10;
	if (actual_vol<0){
		actual_vol=0;
	}
	send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Application.SetVolume","params":{"volume":'+actual_vol+'},"id":1}');
}

function button_mute(){	
	send_json_to_xbmc('/jsonrpc?request={"jsonrpc":"2.0","method":"Application.SetMute","params":{"mute":"toggle"},"id":1}');
}


Pebble.addEventListener("ready",
  function(e) {
    console.log("Preparing...");
	server=window.localStorage.getItem(1);
	user=window.localStorage.getItem(2);
	pass=window.localStorage.getItem(3);
	active_player=window.localStorage.getItem(4);
	if (server=="undefined" || server=="null"||server===undefined || server===null){
		server="http://YOUR_XBMC_IP:PORT";
	}
	if (user=="undefined"|| user=="null" || user===null || user===undefined){
		user="";
	}
	if (pass=="undefined"|| pass=="null"||pass===undefined|| pass===null){
		pass="";
	}
	if (user!==""){
		server_url=server.replace("://","://"+user+":"+pass+"@");
	}
	else{
		server_url=server;
	}
	if (active_player=="undefined"|| active_player=="null" || active_player===undefined|| active_player===null){
		active_player=1;
	}
	//console.log(server_url);
	get_vol();
	console.log("JavaScript app ready and running!");
  }
);


Pebble.addEventListener("showConfiguration", function() {
	console.log("showing configuration");
	server=window.localStorage.getItem(1);
	user=window.localStorage.getItem(2);
	pass=window.localStorage.getItem(3);
	if (server=="undefined" || server=="null"||server===undefined || server===null){
		server="http://XBMC_HOST:8080";
	}
	if (user=="undefined"|| user=="null" || user===null || user===undefined){
		user="";
	}
	if (pass=="undefined"|| pass=="null"||pass===undefined|| pass===null){
		pass="";
	}
	var url='http://46.105.20.128/xbmc_remote_config.php?server='+server+'&user='+user+'&pass='+pass;
	Pebble.openURL(url);
});

Pebble.addEventListener("webviewclosed", function(e) {
	console.log("configuration closed");
	var options = JSON.parse(decodeURIComponent(e.response));
	console.log("Options = " + JSON.stringify(options));
	if (options.server!==undefined){
		console.log("Writting vars");
		if (options.server.slice(-1)=="/"){
			server=options.server.substring(0, options.server.length - 1);
		}
		else{
			server=options.server;
		}
		user=options.user;
		pass=options.pass;
		if (user!==""){
			server_url=server.replace("://","://"+user+":"+pass+"@");
		}
		else{
			server_url=server;
		}
		console.log(server_url);
		window.localStorage.setItem(1, server);
		window.localStorage.setItem(2, user);
		window.localStorage.setItem(3, pass);
	}
});


Pebble.addEventListener("appmessage",
						function(e) {
							switch(e.payload[99])
							{
								case 0:
									button_up();
									break;
								case 1:
									button_down();
									break;
								case 2:
									button_select();
									break;
								case 3:
									button_back();
									break;
								case 4:
									button_left();
									break;
								case 5:
									button_right();
									break;
								case 6:
									button_home();
									break;
								case 7:
									button_title();
									break;
								case 8:
									button_info();
									break;
								case 10:
									button_action_rev();
									get_player_info();
									break;
								case 11:
									button_action_ff();
									get_player_info();
									break;
								case 12:
									button_action_select();
									get_player_info();
									break;
								case 13:
									button_action_long_rev();
									get_player_info();
									break;
								case 14:
									button_action_long_ff();
									get_player_info();
									break;
								case 15:
									button_action_stop();
									get_player_info();
									break;
								case 20:
									get_active_player();
									get_player_info();
									break;
								case 30:
									button_vol_up();
									break;
								case 31:
									button_vol_down();
									break;
								case 32:
									button_mute();
									break;
								default:
									console.log("ERROR, not defined: ");
									console.log(e.payload[1]);
							}
						});
