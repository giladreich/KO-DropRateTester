# KnightOnline Drop Rate Tester
![Logo](/images/rate-tester.png)

I came up with this little program just from learning about how the logic of the drop system MGames implemented, so take as note that it doesn't do anything fancy, just allows you to test the frequency that monsters will drop items at a specific rate and mostly for eduction purpose.
Note that the code is based on the original KnightOnline source code in order to retrieve realistic results.
Another note that in the database we read 100% as 10,000, so the input should be written as normal percentage(so 75% is input of 75).

If you're looking for the exe/binaries download, here it is:
[Download link](https://drive.google.com/open?id=0B0vTRMrlXZn4ZG50VUlVZW9rSXc).

If you're looking for the drop rates for specific items, here it is in excel sheets(token from the 12xx database):
[K_MONSTER / K_MONSTER_ITEM](https://drive.google.com/open?id=1wXmv7-7hdVxacs2Jq3aUrO0TWzy96sKqs3MMZTTojaU)
[MAKE_ITEM_GROUP](https://drive.google.com/open?id=1LqjeOHZM-8lyBHHmrw5QT9gWEpX5IsBqA4vBrWz8KNU)
                                                                                                                                                                                                                                                                                     

I though i should do a little documentation here about how things works and related to each other. so here it is:
* [Database](#database)
* [Client/Server](#clientserver)
* [Useful SQL Queries](#useful-sql-queries)
* [Development](#development)

                                                                                                                                                                                                                                                                                     
## Database
In the database we have 3 important tabels regarding the drop rate which we are interested in:
- ```K_MONSTER``` - Contains all the data on the monster and important for us here is the sSid which is related to the table 2.
- ```K_MONSTER_ITEM``` - Contains all the data regarding the 2x drops groups and 3 different items and depends on the sSid=sIndex on the table 1.
- ```MAKE_ITEM_GROUP``` - iItemGroupNum is the group id which depends on the K_MONSTER_ITEM group id.

So to sum the things above, if we want to have a specific drop to specific monster, we read the sSid in K_MONSTER and we want to add drop to that specific monster which will be declared as the same value in sIndex in K_MONSTER_ITEM table. 
In K_MONSTER_ITEM we have 2 different columns for declaring the drop group id's and 2 columns for their corresponding drop rate, that means, that in each group that can be found by the iItemGroupNum in MAKE_ITEM_GROUP, there are 30 different items.
So if we have 2x groups, that means there is a chance for 60 different items to drop from the same monster as each group/row is 30 different items.
You can find at the end of the document useful queries for displaying what you need.
                                                                                                                                                                                                                                                                                     
## Client/Server
> * Player attack monster
> * Client sending ```AG_NPC_HP_CHANGE``` to the GameServer/Ebenezer and calling the ```CAISocket::RecvNpcHpChange(Packet & pkt)``` with the packet that the client sent.
> * After we call the ```CNpc::HpChange(int amount, Unit *pAttacker, bool bSendToAI)``` in the GameServer and then call the ```CNpc::SendHpChangeToAI(uint16_t sTargetID, int amount, AttributeType attributeType)``` which will after pass forward the packet to the AIServer to do the proccessing of the packet and all the logic of hp change and item drop.
> * Now the AIServer will handle the packet in ```CGameSocket::RecvNpcHpChange(Packet & pkt)```
> * Check if the player damage attack is not 0, if it's not, then we have an attack to proccess, so we call ```CNpc::RecvAttackReq(int nDamage, uint16_t sAttackerID, AttributeType attributeType)```.
> * Then we call the ```CNpc::HpChange(int amount, Unit *pAttacker, bool bSendToGameServer)``` by sending who attacked the monster and how much damage.
> * And then check if the monster hp is 0, if it is 0, we proccess the ```CNpc::Dead(Unit * pKiller, bool bSendDeathPacket)``` function which will afterwards call the last thing we interested in is the ```CNpc::GiveNpcHaveItem()```.
> * The ```CNpc::GiveNpcHaveItem()``` logic says, generate a random number from 1 to 10,000 and then retrieve from the database what is the percentage rate of the item to drop and if the random number generated is greater than the actual number from the database, don't give the player any item, if it's not greater than the actual value from the database, procced the logic of generating all the items from the database to the box.
> * After all that is done, the AIServer will then send a ```AG_NPC_GIVE_ITEM``` packet with all the data results that we need which will contain the items that are in the box and pass to the GameServer.
> * The GameServer will proccess the packet with all the containing items and create a loot bundle of items which will then check if the player is in party(group of other players) or not, that way we see who will be the reciever of the packet with the loot bundle/items and send back all the data to the client as ```WIZ_ITEM_DROP``` packet.
> * At this point, the client already have a box of items, so when the player will right click the box in game, a request from the client to the game server will be sent to retrieve all the bundle of items that are already procced before by the AIServer.

So this is all the logic that happens in the background when we kill a monster and receive a box of items. 
Note that the random item generator that MGames are using is part of the mt19937 random library and they build a wrapper class with static functions that will be used many many times in the server by the "myrand(min, max)" function so that way, we really get a real random numbers since this function gets called in many different cases such as:
attack damage, attack fail, anvil upgrades, item drop and vice versa...
Also everytime we restart the server, this wrapper class they build, will seed/feed/fill the random generator object with the machine time only once! So that way, the random numbers that we receive are not the same default random numbers that we get every restart.
So all these rumors about catching the first upgrade item after server restart to have better success rate OR the person who have premium will loot the box of items to get better items and many more are completely wrong.
The only differences with premium membership that the AIServer will check whether you have premium in the part where you get the amount of coins from the box of items, so premium members will receive more money than normal players.

                                                                                                                                                                                                                                                                                     
## Useful SQL Queries
```sql
-- Shows organized results of the monster id, name, item drop and percentage, item group drop and percentage.
SELECT  
	m.sSid, m.strName, m.sPid,
	mi.iItem03 AS ItemId1, (SELECT strName FROM ITEM WHERE Num =  mi.iItem03) ItemName1, mi.sPersent03 / 100 AS Percentage1, 
	mi.iItem04 AS ItemId2, (SELECT strName FROM ITEM WHERE Num =  mi.iItem04) ItemName2, mi.sPersent04 / 100 AS Percentage2, 
	mi.iItem05 AS ItemId3, (SELECT strName FROM ITEM WHERE Num =  mi.iItem05) ItemName3, mi.sPersent05 / 100 AS Percentage3,
	-- Group ID = MAKE_ITEM_GROUP 30 items that can drop in specific percentage.
	mi.iItem01 AS ##GroupID1, mi.sPersent01 / 100 AS GPercentage1,
	mi.iItem02 AS ##GroupID2, mi.sPersent02 / 100 AS GPercentage1
FROM 
	K_MONSTER AS m
	INNER JOIN 
	K_MONSTER_ITEM AS mi
ON 
	m.sSid = mi.sIndex
WHERE 
	m.strName LIKE '%Snake Queen%'
ORDER BY
	m.sSid


-- Shows the group id, item id item name by the group id selected.

SELECT
	iItemGroupNum AS GroupID,
	iItem_1   AS GItem1,   (SELECT strName FROM ITEM WHERE Num = iItem_1)   GItemName1,
	iItem_2   AS GItem2,   (SELECT strName FROM ITEM WHERE Num = iItem_2)   GItemName2,
	iItem_3   AS GItem3,   (SELECT strName FROM ITEM WHERE Num = iItem_3)   GItemName3,
	iItem_4   AS GItem4,   (SELECT strName FROM ITEM WHERE Num = iItem_4)   GItemName4,
	iItem_5   AS GItem5,   (SELECT strName FROM ITEM WHERE Num = iItem_5)   GItemName5,
	iItem_6   AS GItem6,   (SELECT strName FROM ITEM WHERE Num = iItem_6)   GItemName6,
	iItem_7   AS GItem7,   (SELECT strName FROM ITEM WHERE Num = iItem_7)   GItemName7,
	iItem_8   AS GItem8,   (SELECT strName FROM ITEM WHERE Num = iItem_8)   GItemName8,
	iItem_9   AS GItem9,   (SELECT strName FROM ITEM WHERE Num = iItem_9)   GItemName9,
	iItem_10 AS GItem10, (SELECT strName FROM ITEM WHERE Num = iItem_10) GItemName10,
	iItem_11 AS GItem11, (SELECT strName FROM ITEM WHERE Num = iItem_11) GItemName11,
	iItem_13 AS GItem13, (SELECT strName FROM ITEM WHERE Num = iItem_13) GItemName13,
	iItem_14 AS GItem14, (SELECT strName FROM ITEM WHERE Num = iItem_14) GItemName14,
	iItem_15 AS GItem15, (SELECT strName FROM ITEM WHERE Num = iItem_15) GItemName15,
	iItem_16 AS GItem16, (SELECT strName FROM ITEM WHERE Num = iItem_16) GItemName16,
	iItem_17 AS GItem17, (SELECT strName FROM ITEM WHERE Num = iItem_17) GItemName17,
	iItem_18 AS GItem18, (SELECT strName FROM ITEM WHERE Num = iItem_18) GItemName18,
	iItem_19 AS GItem19, (SELECT strName FROM ITEM WHERE Num = iItem_19) GItemName19,
	iItem_21 AS GItem21, (SELECT strName FROM ITEM WHERE Num = iItem_21) GItemName21,
	iItem_22 AS GItem22, (SELECT strName FROM ITEM WHERE Num = iItem_22) GItemName22,
	iItem_23 AS GItem23, (SELECT strName FROM ITEM WHERE Num = iItem_23) GItemName23,
	iItem_24 AS GItem24, (SELECT strName FROM ITEM WHERE Num = iItem_24) GItemName24,
	iItem_25 AS GItem25, (SELECT strName FROM ITEM WHERE Num = iItem_25) GItemName25,
	iItem_26 AS GItem26, (SELECT strName FROM ITEM WHERE Num = iItem_26) GItemName26,
	iItem_27 AS GItem27, (SELECT strName FROM ITEM WHERE Num = iItem_27) GItemName27,
	iItem_28 AS GItem28, (SELECT strName FROM ITEM WHERE Num = iItem_28) GItemName28,
	iItem_29 AS GItem29, (SELECT strName FROM ITEM WHERE Num = iItem_29) GItemName29,
	iItem_30 AS GItem30, (SELECT strName FROM ITEM WHERE Num = iItem_30) GItemName30
FROM 
	MAKE_ITEM_GROUP
WHERE 
	iItemGroupNum = 2005
ORDER BY
	iItemGroupNum
```

## Development
I'm Using here:
- C++ with Qt Framework version 5.6.
- Visual Studio 2017
- Windows 10

So if you're planing to build this from source, you'll need to download the Qt visual studio pluging version 5.6 and set environment variables to QTDIR.
Otherwise, just get the binaries/executeable from the [download link](https://drive.google.com/open?id=0B0vTRMrlXZn4ZG50VUlVZW9rSXc).