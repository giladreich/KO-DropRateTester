# KnightOnline Drop Rate Tester
![Logo](/pictures/rate-tester.png)

I wrote this simple tool part of my learning about the logic of the drop system MGames is implemented, so take as note that it doesn't do anything fancy, just allows you to test the frequency that monsters will drop items at a specific rate and mostly for eduction purpose.

Note that the code is based on the original KnightOnline source code in order to retrieve realistic results.

Another note that in the database we read 100% as 10,000, so the input should be written as normal percentage(so 75% is input of 75).

If you're looking for the exe/binaries download, here it is:

[Download link](http://www.mediafire.com/file/37ycjgyrcay5v3c/KO-DropRateTester-1.0.0.7z)

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

So if we have 2x groups, that means there is a chance for 60 different items to drop from the same monster as each group/row contains 30 different items.

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
First run the create stored procedure script below and then you can always reuse it like this:

```sql
EXEC sp_MonsterItems '%Isiloon%'
```

```sql
CREATE PROCEDURE sp_MonsterItems
    @MonsterName varchar(30)
AS
DECLARE @GroupIDs TABLE (id int)

-- Shows organized results of the monster id, name, item drop and percentage, item group drop and percentage.
SELECT  
    m.sSid, m.strName, m.sPid,
    mi.iItem03 AS ItemId1, (SELECT strName FROM ITEM WHERE Num =  mi.iItem03) ItemName1,
        CONVERT(DOUBLE PRECISION, mi.sPersent03) / 100 AS Percentage1,
    mi.iItem04 AS ItemId2, (SELECT strName FROM ITEM WHERE Num =  mi.iItem04) ItemName2, 
        CONVERT(DOUBLE PRECISION, mi.sPersent04) / 100 AS Percentage2, 
    mi.iItem05 AS ItemId3, (SELECT strName FROM ITEM WHERE Num =  mi.iItem05) ItemName3, 
        CONVERT(DOUBLE PRECISION, mi.sPersent05) / 100 AS Percentage3, 
    -- Group ID = MAKE_ITEM_GROUP 30 items that can drop in specific percentage.
    mi.iItem01 AS ##GroupID1, CONVERT(DOUBLE PRECISION, mi.sPersent01) / 100 AS GPercentage1,
    mi.iItem02 AS ##GroupID2, CONVERT(DOUBLE PRECISION, mi.sPersent02) / 100 AS GPercentage1
FROM 
    K_MONSTER AS m
    INNER JOIN 
    K_MONSTER_ITEM AS mi
ON 
    m.sSid = mi.sIndex
WHERE 
    m.strName LIKE @MonsterName
ORDER BY
    m.sSid

-- Storing all GroupID's into variables.
INSERT INTO @GroupIDs(id) SELECT DISTINCT mi.iItem01 FROM K_MONSTER AS m INNER JOIN K_MONSTER_ITEM AS mi 
ON  m.sSid = mi.sIndex WHERE m.strName LIKE @MonsterName AND mi.iItem01 > 0

INSERT INTO @GroupIDs(id) SELECT DISTINCT mi.iItem02 FROM K_MONSTER AS m INNER JOIN K_MONSTER_ITEM AS mi 
ON  m.sSid = mi.sIndex WHERE m.strName LIKE @MonsterName AND mi.iItem02 > 0

DECLARE @GroupIDsClean TABLE (id int);
INSERT INTO @GroupIDsClean SELECT DISTINCT * FROM @GroupIDs

DECLARE @c int = 1,
        @IdsCount int;
SELECT @IdsCount = COUNT(id) FROM @GroupIDsClean

WHILE @c <= @IdsCount
BEGIN
    DECLARE @iItemGroupNum int;
    SELECT @iItemGroupNum = id FROM (SELECT ROW_NUMBER() OVER (ORDER BY id) AS RowNum, id 
    FROM @GroupIDsClean) AS t WHERE t.RowNum = @c;
    
    DECLARE @columns     nvarchar(3000) = '',
            @params      nvarchar(500),
            @query       nvarchar(4000),
            @i           int = 1,
            @columnCount int;
             
    SELECT @columnCount = COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME = N'MAKE_ITEM_GROUP';
    EXEC sp_addmessage 50001, 16, '%s GItem%d,(SELECT strName FROM ITEM WHERE Num = iItem_%d) GItemName%d,', NULL, NULL, 'replace';
    WHILE @i < @columnCount
    BEGIN
        SELECT @columns += FORMATMESSAGE(50001, COLUMN_NAME, @i, @i, @i)
        FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME = N'MAKE_ITEM_GROUP' AND ORDINAL_POSITION = @i + 1;
        SET @i += 1;
    END
    SELECT @columns = SUBSTRING(@columns, 0, LEN(@columns));

    SET @columns = 'iItemGroupNum GroupID,' + @columns;
    SET @params = '@iItemGroupNum INT';
    SET @query = 
    'SELECT
        ' + @columns + ' 
    FROM 
        MAKE_ITEM_GROUP
    WHERE
        iItemGroupNum = @iItemGroupNum
    ORDER BY
        iItemGroupNum';
    EXEC sp_executesql @query, @params, @iItemGroupNum
    
    SET @c += 1;
END
```

## Development
I'm Using here:
- C++ with Qt Framework version 5.6.
- Visual Studio 2017
- Windows 10

So if you're planing to build this from source, you'll need to download Qt for visual studio(version 5.6, also plugin) and set environment variables to QTDIR or you can build the project using qmake(works almost the same as cmake).
Otherwise, just get the binaries/executeable from the [download link](http://www.mediafire.com/file/37ycjgyrcay5v3c/KO-DropRateTester-1.0.0.7z).