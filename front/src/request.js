const net = require('net');

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

const tcpClient = {
    options : {
        host: '127.0.0.1',
        port: 12345
    },
    socket: net.Socket(),
    init : function(){
        this.socket.connect(this.options,function(){
            console.log("connected to server");
            //tcp_client.write('yay!');
        })
    }
}

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
    "RegularResponse":null,
    "Req":null,
    "Res":null
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
    root = await protobuf.load("./Wrapper.proto");
    protoStorage["Req"] = root.lookupType("WrapperClientMessage");
    protoStorage["Res"] = root.lookupType("WrapperServerMessage");
}

//后端api
function ProtobufSend(messageName,payload){
    let message = protoStorage[messageName].create(payload);
    console.log(message);
    let bytes = protoStorage[messageName].encode(message).finish();
    console.log(bytes);
    tcpClient.socket.write(bytes);
    //let re = protoStorage[messageName].decode(bytes);
    //console.log(re);
}

const ProtobufReciever = {
    _onRecieveMessageResponse:[],
    _onRecieveRoomResponse:[],
    _onRecieveAuthResponse:undefined,
    _onChatMessageRecieve:function(e){console.log(e)},
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

var isLogin = false;

function ProtobufRead(payload){
    var res = protoStorage["Res"].decode(payload);
    console.log(res);
    if(res.loginResponse){
        ProtobufReciever._onRecieveAuthResponse(res.loginResponse);
        if(res.loginResponse.username){
            userState.username = res.loginResponse.username;
            userState.tokenFromServer = res.loginResponse.token;
        }
    }else if(res.chatMessageRequest){
        for(let i = 0 ;i<ProtobufReciever._onRecieveMessageResponse.length;i++){
            ProtobufReciever._onRecieveMessageResponse[i]();
        }
    }else if(res.ChatMessageSend){
        ProtobufReciever._onChatMessageRecieve(res.ChatMessageSend);
    }
    /*
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

    }*/
    else{
        console.log("unsupported message type.");
    }
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
            try{
                ProtobufSend("Req",{
                    test: 1,
                    loginRequest:{
                    username : username,
                    password : password
                    }
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
                ProtobufSend("Req",{
                    test: 1,
                    registerRequest:{
                    username : username,
                    password : password,
                    showname : showname
                    }
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
                ProtobufSend("Req",{
                    test:1,
                    chatMessageRequest:{
                    username : userState.loginUserName,
                    token : userState.tokenFromServer,
                    msg : msg,
                    roomid : roomid,
                    random : md5(msg)
                    }
                });
            }catch(e){
                reject(e);
            }
        }),5000);
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

exports.StartClient = function(){
    LoadAllProto().then(()=>{
        tcpClient.init();
        tcpClient.socket.on("data",(data)=>{ProtobufRead(data);});
    });
    
}

exports.OnChatMessageRecieve = function(cb){
    ProtobufReciever._onChatMessageRecieve = cb;
}
