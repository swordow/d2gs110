# Diablo II Game Server (D2GS)
This repository contains changes to [pvpgn/d2gs109](https://github.com/pvpgn/d2gs109) that hope to make it work with margsod 1.11b build 46.

I decompiled the d2gs.exe used in margsod 1.11b build 46 and updated the code logic mostly, but it crashed somewhere, and i dont have much time to fix the bug(:-(, some overflow occurs in some logic).

## History from RElesgoe's d2gs repo

>## How Does D2GS Work
>The game server application loads `D2Server.dll`, calls `QueryInterface()`, and then calls the start >function using one of the function pointers returned by `QueryInterface()`. Once started, `D2Server.dll` >sets up network processing and performs calls to DLLs such as `Fog.dll` and `D2Game.dll` to create and run >game instances.
>
>## History
>Blizzard accidentally included `D2Server.dll` in distributions of Diablo II 1.00, but have removed the file >in subsequent installers and patches. Using `D2Server.dll`, Onlyer developed a working game server in late >2000 and eventually released D2GS in 2001. D2GS was actively developed between 2001 and 2002, adding support >for patches 1.09c and 1.09d. Several months after Diablo II patch 1.10 was released, Onlyer released the >final D2GS version, D2GS 1.10 - Build 6 on March 4, 2004.
>
>Because Onlyer didn't release the source code for D2GS, D2GS servers were stuck with supporting patch 1.10 >as patches 1.11 and 1.11b became released in 2005. In 2007, Marsgod began distributing D2GS with support for >patch 1.11b and eventually supported patches 1.13a and 1.13c. Marsgod achieved support for these patches by >compiling hand written assembly and copying the code section of the compiled program into `D2Server.dll`.
>
>At some point, the source code for D2GS 1.09d was released at [d2dev.dlg.cn](http://d2dev.dlg.cn). Mirrors >have been uploaded at https://github.com/pvpgn/d2gs109 and http://www.pudn.com/Download/item/id/884918.html. >Despite the release of the source code, virtually all D2GS servers still rely on Marsgod's distributions >because of the lack of development using the source code. Only one person, [tesseract2048](https://github.>com/tesseract2048/), has been known to [update the source code](https://github.com/tesseract2048/d2gs) and >use it in production, though its known to be buggy and it requires a customized version of `d2cs` and >`d2dbs`.
>
>## Protocol Documentation
>There isn't a single source that documents all known packets.
>
>- https://github.com/MephisTools/diablo2-protocol/wiki/packets
>- https://bnetdocs.org/packet/index?order=packet-id-asc&pktapplayer%5B%5D=4
>- https://d2mods.info/forum/viewtopic.php?t=62821
>- https://blizzhackers.github.io/Diablo2PacketsData/
>
>
>## A Message From Onlyer
>> About the Diablo2 Close Battle.net Server
>>
>> As you can see, I and my partners made a Battle.net server with Diablo2 close game support last year. I >will try to explain how it works and what is the problem with it. 
>>
>> First, I will show why and how I made that server. 
>>
>> Last year, after Diablo2 comes out, I know that game from friends and was soon interested in it. Soon, I >am tired with single player game, and so changes to play multiplayer > game on our local lan. in these days, >I heard of a software named FSGS can help to create a battle.net server for Diablo2, that makes multiplayer >game more easy and interesting. So I setup a server with FSGS on lan, and from FSGS's web page, I know that >they are developing with the Diablo2 close server. it will be exciting if there are close mode support for >this game, for our net limit, we can not go abord directly. And it will be very lag to play it on official >battle.net through proxy. So I waited and hope FSGS can make it out soon. 
>>
>> After a long time wait, no good news from FSGS, and it keeps saying -- it will be out, wait and see... In >these days, I found that FSGS is spawned from BNETD, which also can make a battle.net server. And the most >important, BNETD is open source. So I decided try to make a own server with Diablo2 close support by >modifying current BNETD code. 
>>
>> I downloaded the source and compiled it, start the server. all works fine. so I changed all current FSGS >server user account to BNETD format. change server to BNETD to test. in the test, I found BNETD is stable >and begin to write own code. 
>>
>> I finished the battle.net part support for Diablo2 in short time, for BNETD source code is easy to >understand and add new future. I should thanks all BNETD developer here, their hard work saved me lots of >time. Then I began with the game server part. 
>>
>> As we know, the most important thing in Diablo2 close support is game server, unlike previous games from >Blizzard, in close mode, the game server is in server instead of client side. The client just send player >action to server, and server should reply the result in correct format. Blizzard do not make their game >server to public, so it will be hard to program a game server in your own code. In this case, you should >guess out all client/server communication protocol and then make program a Diablo2 game without client GUI >interface. I decided to use another way. 
>>
>> You can find out easily that in tcp/ip mode games, the host just act as a game server. So the way i choose >is using the tcp/ip game host as game server. The battle.net server listen at game server port, receive >client request, do some necessary parse, get the imformation, do some change when needed, pass the request >to a tcp/ip game host. The game host will respones to the request, the battle.net server will receive the >respones, do some parse again here. Then pass it to the client, just works like a proxy or tunnel. There is >a tool called bnproxy in BNETD utils, so I merged bnproxy with BNETD, bnproxy act as tunnel to game server. >BNETD for battle.net server as before, and after lots of failure and try, I find that works as I image at >first. The close game server use a nearly same protocol with open game host. This is the initial model of >the server. 
>>
>> At that time, the game server only supports one game as tcp/ip host, that sounds very bad, isn't it? But >soon, I found out some hidden game protocol that can create more than one game in one host, and with >modifying some paramters in the request, the game server can act much like official game server.
>>
>> At the end of last year, after some weeks debug and test, the server based on this model becomes much >stable than begining, so i put it out. In these days, some friends joined the development, and also lots of >friends help to test and maintain the server. I should say great thanks to them. 
>>
>> But still, there are lots of problems and bugs in the server based on this model. and lots of code are >from scratch. so I and my partners planed to change current server architecture. but with time limit and >some other reason, it goes on slowly and seems stopped. 
>>
>> And currently, I plans to combine my old project into BNETD project, that will save time and avoid repeat >work, hope we can make it better together. 
>>
>> Comments and suggestions are welcomed to onlyer@263.net BTW: the old 1.03 server code is a tempory >solution, and nearly useless now, no need to waste time to read it.
>>
>> ***onlyer***
>>
>> 2001-08-13

## License
```
Copyright (C) 2000,2001
	faster (faster@263.net)
	onlyer (onlyer@263.net£©
>
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
>
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
>
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
```