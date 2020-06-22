//
//  HTML PAGE
//

const char PAGE_AdminGeneralSettings[] PROGMEM =  R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>General Configuration</strong>
<hr>
<form action="" method="post">
<hr>
<strong>Administrator Credentials</strong><br>
<table border="0"  cellspacing="0" cellpadding="3" >
<tr>
	<td align="right">Username:</td>
	<td><input type="text" id="webusername" name="webusername" value=""></td>
</tr>
<tr>
	<td align="right">Password:</td>
	<td><input type="text" id="webpassword" name="webpassword" value=""></td>
</tr>

</table>
<hr>
<strong>Device info</strong><br>
<table border="0"  cellspacing="0" cellpadding="3" >
<tr><td align="right">Temperature:</td><td><span id="temperature"></span></td></tr>
<tr><td align="right">Humidity:</td><td><span id="humidity"></span></td></tr>
<tr><td align="right">sw Version:</td><td><span id="sw_Version"></span></td></tr>
<tr><td align="right">Mac:</td><td><span id="x_mac"></span></td></tr>
<tr><td align="right">AP SSID:</td><td><span id="ap_ssid"></span></td></tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<script>



window.onload = function ()
{
	load("style.css","css", function()
	{
		load("microajax.js","js", function()
		{
				setValues("/admin/generalvalues");
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====";


// Functions for this Page
void send_general_html()
{

	if (MyWebServer.args() > 0 )  // Save Settings
	{
		String temp = "";
		for ( uint8_t i = 0; i < MyWebServer.args(); i++ ) {
			if (MyWebServer.argName(i) == "webusername") strcpy(config.WEB_User, urldecode(MyWebServer.arg(i)).c_str());
			if (MyWebServer.argName(i) == "webpassword" && urldecode(MyWebServer.arg(i)) != "") strcpy(config.WEB_Password, urldecode(MyWebServer.arg(i)).c_str());
		}
		storage_write();
		firstStart = true;
	}
	if (!MyWebServer.authenticate(config.WEB_User, config.WEB_Password))  {
		MyWebServer.requestAuthentication();
	}
	else{
		MyWebServer.send ( 200, "text/html", PAGE_AdminGeneralSettings );
	}
	MyWebServer.send ( 400, "text/html", "Authentication failed" );
	Serial.println(__FUNCTION__);


}

void send_general_configuration_values_html()
{
	String values ="";
	values += "webusername|" +  String(config.WEB_User) +  "|input\n";
	//values += "webpassword|" +  String(config.WEB_Password) +  "|input\n";  	// KEEP IT COMMENTED TO NOT SHOW THE WiFi KEY!!!
 	values += "temperature|" + String(getTemperature()) +  "|div\n";
 	values += "humidity|" + String(getHumidity()) +  "|div\n";
 	values += "sw_Version|" + String(SWVer) +  "|div\n";
 	values += "x_mac|" + GetMacAddress() +  "|div\n";
    values += "ap_ssid|" + ESP_SSID +  "|div\n";
	MyWebServer.send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__);
}
