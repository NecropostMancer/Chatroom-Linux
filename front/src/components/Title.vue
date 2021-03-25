<template>
    <div id="root">
        <div id="login-page" v-if="isLogin">
            <h1>Join!</h1>
            <div>
                <span class="name">用户名</span><br>
                <input id="username" v-model="username"/>
            </div>
            <div>
                <span class="name">密码</span><br>
                <input id="password" type="password" v-model="pass"/>
            </div>
            <div id= "click">
                <span class="button" id="login-btn" v-on:click="login">登录</span>
                <span id = "forget">忘记密码?</span>
            </div>
            
            <div id ="rem">
                <input type="checkbox" style="display: inline;">
                <span>记住我</span>
                
            </div>
            <div class = "notice-item">
                <span class="font-notice">没有账号?</span>
            </div>
            <div id = "reg" v-on:click="isLogin = !isLogin">
                立刻注册
            </div>
        </div>
        <div id="reg-page" v-else>
            <h1>注册</h1>
            <div>
                <span class="name">用户名</span><br>
                <input id="username-new" v-model="username"/>
            </div>
            <div>
                <span class="name">昵称</span><br>
                <input id="showname" v-model="name"/>
            </div>
            <div>
                <span class="name">密码</span><br>
                <input id="password-new" type="password" minlength="6" v-model="pass"/>
            </div>
            <div>
                <span class="name">确认密码</span><br>
                <input id="password-new-re" type="password" minlength="6" v-model="passver"/>
            </div>
            <div id= "reg-click">
                <span id = "reg-btn" class = "button" v-on:click="register">注册</span>
            </div>
        </div>
    </div>
</template>
<script>
    export default{
        data(){
            return {
                isLogin: true,
                username:"",
                pass:"",
                passver:"",
                name:""
            }
        },
        mounted () {
            console.log(this.request);
        },
        methods:{
            findError : function(code){
                console.log(code);
                switch(code){
                    case undefined:
                        return true;
                    case 0:
                        return false;
                    case 2:
                        return true;
                    case 3:
                        return true;
                    case 4:
                        return true;
                }
            },
            login : function(){
                this.request.LoginRequest(this.username,this.pass).then((res)=>{
                    console.log(res.error);
                    if(!this.findError(res.error.code)){
                        this.$userState.loginUserName = this.username;
                        this.$userState.tokenFromServer = res.token;
                        this.$userState.loginShowName = res.token;
                        
                        this.request.SwitchPage();
                    }else{
                        this.showError("bruh");
                    }
                }).catch((e)=>{
                    console.log(e);
                })
            },
            register : function(){
                this.request.RegisterRequest(this.username,this.pass,this.name).then((res)=>{
                    if(!this.findError(res.error.code)){
                        this.isLogin = true;
                    }else{
                        this.showError("bruh");
                    }
                    
                }).catch((e)=>{
                    this.showError("bruh");
                    this.isLogin = true;
                })
            },
            showError : function(e){
                console.log(e);
            }
        },
        props :{
            request : Object
        }
    }
</script>

<style scoped lang="scss">
    html, body {
        width: 100%;
        height: 100%;
        margin: 0;
        padding: 0;
        user-select: none;
    }
    #root{
        width: 100%;
        text-align: left; 
        margin: auto;
        padding-top: 30px;
    }
    #root h1{
        text-align: center;
    }
    div{
        width: 300px;
        margin: auto;
        padding-top: 10px;
    }
    div input{
        width: 100%;
        height: 32px;
        padding-left: 10px;
        margin-top: 5px;
    }
    div input[type = "checkbox"]{
        display: inline;
        width: auto;
        height: auto;
        margin: 0;
    }
    .name{
        color:gray;
    }

    .notice-item {
        position: relative;
        margin-top: 10px;
        margin-bottom: 10px;
        text-align: center;
        overflow: hidden;
        min-height: 32px;
        color: black;
        font-size: small;
    }

    .notice-item::before,
    .notice-item::after {
        background-color: darkgray;
        content: "";
        display: inline-block;
        height: 1px;
        position: relative;
        vertical-align: middle;
        width: 50%;
    }

    .notice-item::before {
        right: 0.5em;
        margin-left: -50%;
    }
    .notice-item::after {
        left: 0.5em;
        margin-right: -50%;
    }

    #click .button{
        display: inline-block;
        border-radius: 25px;
        padding: 10px;
        padding-left: 25px;
        padding-right: 25px;
        margin-right: 160px;
        color: white;
        background-color: rgb(64, 29, 192);
    }
    #reg-click .button{
        display: inline-block;
        border-radius: 25px;
        padding: 10px;
        padding-left: 45px;
        padding-right: 45px;
        color: white;
        background-color: rgb(64, 29, 192);
    }
    #click .button:hover{

    }

    #click span{
        color: cornflowerblue;
        font-size: small;
    }
    #rem span{
        color:cornflowerblue;
        font-size: small;
    }
    #rem input{
        color: cornflowerblue;
    }
    #reg{
        text-align: center;
        color:cornflowerblue;
    }
    .hide{
        display: none;
    }

    #reg-page #reg-click{
        text-align: center;
        width: 100%;
    }
</style>