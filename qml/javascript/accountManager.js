var db = null;

// init DB
function initDatabase()
{
    console.log("db, initDatabase()")
    if(db === null)
    {
        db = openDatabaseSync("MReaderDB", "1.0", "DB for MReader1.0", 1000000);
        console.log("Create MReaderDB...");
    }

    db.transaction(
        function(tx)
        {
            tx.executeSql('CREATE TABLE IF NOT EXISTS Account(username TEXT, password TEXT)');
        }
    )
}

// clear all records, actually, only one user we save.
function clearDatabase()
{
    console.log("db, clearDatabase()")
    if(db === null)
    {
        db = openDatabaseSync("MReaderDB", "1.0", "DB for MReader1.0", 1000000);
    }
    db.transaction(
        function(tx)
        {
            tx.executeSql('DROP TABLE IF EXISTS Account');
        }
     )
}

function loadUserNameList(userList)
{
    console.log("db, loadUserNameList()")

    var userNameArr = new Array;

    initDatabase();
    var ret = false;

    db.transaction(
        function(tx)
        {
             var rs = tx.executeSql('SELECT username FROM Account')
            console.log("SELECT username Result: " + rs.rows.length);

            if(rs.rows.length > 0)
            {
                var index = 0;
                while (index < rs.rows.length)
                {
                    userNameArr.push(rs.rows.item(index))
                    ++index;
                }
                userList = userNameArr;
                ret = true;
            }
        }
    );

    return ret;
}

function getPwdByUserName(userName)
{
    if(0 === userName.length)
    {
        return;
    }

    initDatabase();
    db.transaction(
        function(tx) {
              var rs = tx.executeSql('SELECT password FROM Account WHERE username = ?', userName)
              if(rs.rows.length > 0)
              {
                  return rs.rows.item(0);
              }
        }
    )
}

// load user's info
function loadUserInfo(userInfo)
{
    console.log("db, loadUserInfo()")
    initDatabase();
    var ret = false;

    db.transaction(
        function(tx)
        {
             var rs = tx.executeSql('SELECT * FROM Account')
            console.log("SELECT Result: " + rs.rows.length);

            if(rs.rows.length > 0)
            {
                userInfo.userName = rs.rows.item(0).username;
                //password should be decode
                userInfo.password = rs.rows.item(0).password;

                console.log("user_name:" +userInfo.userName);
                console.log("passwd:" + userInfo.password);

                ret = true;
            }
        }
    );

    return ret;
}

function createUserInfo(userInfo)
{
    console.log("db, createUserInfo()")

    console.log("createConsumer userInfo.username is " + userInfo.userName + "userInfo. password is " + userInfo.password)

    initDatabase();

    // if no input, return flase
    if((0 === userInfo.userName.length) || (0 === userInfo.password.length))
    {
        return false;
    }

    db.transaction(
        function(tx)
        {
            var rs = tx.executeSql('SELECT * FROM Account where username="' + userInfo.userName + '"');
            console.log("rs.rows.length　is  " + rs.rows.length);
            if(rs.rows.length > 0)
            {
                //update
                console.log("update")
                tx.executeSql('UPDATE Account SET password = ? WHERE username = ?', [ userInfo.password, userInfo.userName]);
            }
            else
            {
                console.log("insert")
                //delete all records first
                tx.executeSql('DELETE FROM Account');
                tx.executeSql('INSERT INTO Account VALUES(?, ?)', [userInfo.userName, userInfo.password]);
            }
        }
    )
}


//----------------sohu part--------------

function initSohuDB()
{
    console.log("db, initDatabase()")
    if(db === null)
    {
        db = openDatabaseSync("SohuMiniBlogDB", "1.0", "DB for save user's info", 1000000);
        console.log("Create SohuMiniBlogDB...");
    }

    db.transaction(
        function(tx)
        {
            tx.executeSql('CREATE TABLE IF NOT EXISTS SohuAccount(username TEXT, password TEXT)');
        }
    )
}

function createSohuUserInfo(userInfo)
{
    console.log("db, createSohuUserInfo()")

    console.log("createConsumer userInfo.username is " + userInfo.userName + "userInfo. password is " + userInfo.password +  userInfo.userId)

    initSohuDB();

    // if no input, return flase
    if((0 === userInfo.userName.length) || (0 === userInfo.password.length) )
    {
        return false;
    }

    db.transaction(
        function(tx)
        {
            var rs = tx.executeSql('SELECT * FROM SohuAccount where username="' + userInfo.userName + '"');
            console.log("rs.rows.length　is  " + rs.rows.length);
            if(rs.rows.length > 0)
            {
                //update
                console.log("update")
                tx.executeSql('UPDATE SohuAccount SET password = ? WHERE username = ?', [ userInfo.password, userInfo.userName]);
            }
            else
            {
                console.log("insert")
                //delete all records first
                tx.executeSql('DELETE FROM SohuAccount');
                tx.executeSql('INSERT INTO SohuAccount VALUES(?, ?)', [userInfo.userName, userInfo.password]);
            }
        }
    )
}

// load user's info
function loadSohuUserInfo(userInfo)
{
    console.log("db, loadSohuUserInfo()")
    initSohuDB();
    var ret = false;

    db.transaction(
        function(tx)
        {
             var rs = tx.executeSql('SELECT * FROM SohuAccount')
            console.log("SELECT Result: " + rs.rows.length);

            if(rs.rows.length > 0)
            {
                userInfo.userName = rs.rows.item(0).username;
                //password should be decode
                userInfo.password = rs.rows.item(0).password;

                console.log("user_name:" +userInfo.userName);
                console.log("passwd:" + userInfo.password);

                ret = true;
            }
        }
    );

    return ret;
}

function clearSohuDatabase()
{
    console.log("db, clearSohuDatabase()")
    if(db === null)
    {
        db = openDatabaseSync("SohuMiniBlogDB", "1.0", "DB for MReader1.0", 1000000);
    }
    db.transaction(
        function(tx)
        {
            tx.executeSql('DROP TABLE IF EXISTS SohuAccount');
        }
     )
}
