const net = require('net');

var options = {
    host: '127.0.0.1',
    port: 12345
};

var tcp_client = net.Socket();

tcp_client.connect(options,function(){
    console.log("connected to server");
    //tcp_client.write('yay!');
})
/*
tcp_client.on("data",function(data){
    console.log("%s",data.toString());
})

tcp_client.on("end",function(){
    console.log('end.');
})

tcp_client.on("error",function(error){
    console.log('error');
})
*/

/**
 * 一个合理的设计：
 * 读入一个信息后，进行如下操作：
 * 查询发送的房间
 * 查出所有房间中所有用户的socket
 * 挨个发送
 */

const protobuf = require('protobufjs');
const md5 = require('md5');

//测试
protobuf.load("./user.proto",function(err, root){
    if(err){
        throw err;
    }

    const Register = root.lookupType("RegisterRequest");
    let payload = {username:"234",password:"234"};
    let message = Register.create(payload);
})
//初始化proto
const protoStorage = {
    "ChatMessageRequest":null,
    "ChatMessageSend":null,
    "ChatResponse":null,
    "Error":null,
    "RoomRequest":null,
    "RoomResponse":null,
    "RoomControl":null,
    "LoginRequest":null,
    "LoginResponse":null,
    "RegisterRequest":null,
    "ChangeNameRequest":null,
    "RegularResponse":null
}

const userState = {
    loginUserName:"",
    tokenFromServer:"",
    loginShowName:""
}

async function LoadAllProto(){
    let root = await protobuf.load("./Chat.proto");
    protoStorage["ChatMessageRequest"] = root.lookupType("ChatMessageRequest");
    protoStorage["ChatMessageSend"] = root.lookupType("ChatMessageSend");
    protoStorage["ChatResponse"] = root.lookupType("ChatResponse");
    root = await protobuf.load("./Error.proto");
    protoStorage["Error"] = root.lookupType("Error");
    root = await protobuf.load("./Room.proto");
    protoStorage["RoomRequest"] = root.lookupType("RoomRequest");
    protoStorage["RoomResponse"] = root.lookupType("RoomResponse");
    protoStorage["RoomControl"] = root.lookupType("RoomControl");
    root = await protobuf.load("./User.proto");
    protoStorage["LoginRequest"] = root.lookupType("LoginRequest");
    protoStorage["LoginResponse"] = root.lookupType("LoginResponse");
    protoStorage["RegisterRequest"] = root.lookupType("RegisterRequest");
    protoStorage["ChangeNameRequest"] = root.lookupType("ChangeNameRequest");
    protoStorage["RegularResponse"] = root.lookupType("RegularResponse");
}
LoadAllProto();
//后端api
function ProtobufSend(messageName,payload){
    let message = protoStorage[messageName].create(payload);
    console.log(message);
    tcp_client.write(protoStorage[messageName].encode(message).finish());
    
}

tcp_client.on("data",(data)=>{ProtobufRead(data);});

const ProtobufReciever = {
    _onRecieveMessageResponse:[],
    _onRecieveRoomResponse:[],
    _onRecieveAuthResponse:undefined,
    onRecieveMessageResponse:function(cb){
        this._onRecieveMessageResponse.push(cb);
    },
    onRecieveRoomResponse:function(cb){
        this._onRecieveRoomResponse.push(cb);
    },
    removeMessageResponse:function(cb){
        let ind = this._onRecieveMessageResponse.indexOf(cb);
        this._onRecieveMessageResponse.splice(ind,1);
    },
    removeRoomResponse:function(cb){
        let ind = this._onRecieveRoomResponse.indexOf(cb);
        this._onRecieveRoomResponse.splice(ind,1);
    },
    onAuth(cb){
        this._onRecieveAuthResponse = cb;
    }
}

var ExpectedValidationResponse = undefined;

function ProtobufRead(payload){
    if(payload instanceof protoStorage["ChatMessageSend"]){

    }else if(payload instanceof protoStorage["ChatMessageResponse"]){

    }else if(payload instanceof protoStorage["Error"]){

    }else if(payload instanceof protoStorage["RoomResponse"]){

    }else if(payload instanceof protoStorage["LoginResponse"]){
        userState.tokenFromServer = payload.token;
        userState.loginUserName = payload.username;
        userState.loginShowName = "not defined";
        ExpectedValidationResponse = "fulfilled";
        ProtobufReciever._onRecieveAuthResponse();
    }else if(payload instanceof protoStorage["RegularResponse"]){

    }else{
        console.log("unsupported message type.");
    }
}

function OnMessage(){

}

function delayPromise(ms){
    return new Promise(function(resolve){
        setTimeout(resolve,ms);
    })
}

function timeoutPromise(promise,ms) {
    let timeout = delayPromise(ms).then(function(){
        throw new Error('time out.');
    });
    return Promise.race([promise,timeout]);
}

function ResolveCurrentEvent(){

}

exports.LoginRequest = function (username,password){
    return timeoutPromise(
        new Promise(function(resolve,reject){
            ProtobufReciever.onAuth(function(response){
                resolve(response); 
            });
            console.log("send")
            try{
                ProtobufSend("LoginRequest",{
                    username : username,
                    password : password
                });
            }catch(e){
                reject(e);
            }
       }),5000);
    
}

exports.RegisterRequest = function (username,password,showname){
    
    return timeoutPromise(
        new Promise(function(resolve,reject){
            ProtobufReciever.onAuth(function(response){
                resolve(response); 
            });
            try{
                ProtobufSend("RegisterRequest",{
                    username : username,
                    password : password,
                    showname : showname
                });
            }catch(e){
                reject(e);
            }
       }),5000);
}

exports.ChatMessageRequest = function (msg,roomid){
    return timeoutPromise(
        new Promise(function(resolve,reject){
            ProtobufReciever.onRecieveMessageResponse(function(response){
                if(yes,itsWhatMessageImWaitingFor){//<--implmenting this one.
                    ProtobufReciever.removeMessageResponse(this);
                    resolve(response);
                }
            });
            try{
                ProtobufSend("ChatMessageRequest",{
                    username : userState.loginUserName,
                    token : userState.tokenFromServer,
                    msg : msg,
                    roomid : roomid,
                    random : md5(msg)
                });
            }catch(e){
                reject(e);
            }
        }),30000);
}

exports.RoomRequest = function(opreation,roomid){
    var op = undefined;
    switch (opreation){
        case 0:
            op = protoStorage["RoomRequest"].Operation.CREATE;
            break;
        case 1:
            op = protoStorage["RoomRequest"].Operation.JOIN;
            break;
        case 2:
            op = protoStorage["RoomRequest"].Operation.LEAVE;
            break;
    }
    ProtobufSend("RoomRequest",{
        roomid : roomid,
        username : userState.loginUserName,
        opreation : op
    })
}

exports.RoomControl = function(opreation,roomid,extra){
    var op = undefined;
    switch (opreation){
        case 0:
            op = protoStorage["RoomControl"].Operation.LOCK;
            break;
        case 1:
            op = protoStorage["RoomControl"].Operation.UNLOCK;
            break;
        case 2:
            op = protoStorage["RoomControl"].Operation.MUTE;
            break;
        case 3:
            op = protoStorage["RoomControl"].Operation.PROMOTE;
            break;
    }
    ProtobufSend("RoomControl",{
        roomid : roomid,
        username : userState.loginUserName,
        opreation : op,
        payload : extra
    })
}


/*
//测试
//testProtobuf();
LoadAllProto().then(() =>{
    tcp_client.connect(options,function(){
        console.log("connected");
        LoginRequest("123","123");
        RegisterRequest("username","password","showname");
        ChatMessageRequest("msg",600);
        RoomRequest(0,2);
    })
    

})

*/