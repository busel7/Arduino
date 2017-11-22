#ifndef header_h
#define header_h

String file1=
"<!DOCTYPE html>\r\n"
"<html lang=\"en\">\r\n"
"    <head>\r\n"
"        <meta charset=\"utf-8\">\r\n"
"        <title>Grobak.Net - NodeMCU Web Page Text</title>\r\n"
"        <script>\r\n"
"\r\n"
"        strLine = \"\";\r\n"
"        \r\n"
"        function SendText()\r\n"
"        {\r\n"
"            var request = new XMLHttpRequest();\r\n"
"            \r\n"
"            strLine = document.getElementById(\"txt_form\").line.value;\r\n"
"            \r\n"
"            request.open(\"GET\", \"text/\" + strLine + \"/\", true);\r\n"
"            request.send(null);\r\n"
"        }\r\n"
"        </script>\r\n"
"    </head>\r\n"
"\r\n"
"    <body>\r\n"
"        <p><b>Enter text to send to Display:</b></p>\r\n"
"        <form id=\"txt_form\" name=\"frmText\">\r\n"
"            <label>Text: <input type=\"text\" name=\"line\" size=\"60\" maxlength=\"2000\" /></label>\r\n"
"        </form>\r\n"
"        <br />\r\n"
"        <input type=\"submit\" value=\"Send Text\" onclick=\"SendText()\" />\r\n"
"    </body>\r\n"
"\r\n"
"</html>\r\n";

#endif
