<template>
    <div id="root">
        <div id="login-page" v-if="isLogin" class="block">
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
                <span class="font-notice">需要新的账号?</span>
            </div>
            <div id = "reg" v-on:click="isLogin = !isLogin">
                注册
            </div>
        </div>
        <div id="reg-page" class="block" v-else>
            <button type="button" class="btn-close btn-close-white" aria-label="Close" v-on:click="back"></button>
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
                name:"",
                radius:1500,
                expanding:false
            }
        },
        mounted () {
            console.log(this.request);
            this.createAimation();
        },
        methods:{
            createAimation(){
                const TWO_PI = Math.PI * 2;
                const HALF_PI = Math.PI / 2;
                const canvas = document.createElement("canvas");
                const c = canvas.getContext("2d");

                canvas.width = window.innerWidth;
                canvas.height = window.innerHeight - 30;
                document.getElementById("root").appendChild(canvas);
                var that = this;
                class Blob {
                constructor() {
                    this.wobbleIncrement = 0;
                    // use this to change the size of the blob
                    this.radius = that.$data.radius;
                    // think of this as detail level
                    // number of conections in the `bezierSkin`
                    this.segments = 12;
                    this.step = Math.PI / this.segments;
                    this.anchors = [];
                    this.radii = [];
                    this.thetaOff = [];

                    const bumpRadius = 100;
                    const halfBumpRadius = bumpRadius / 2;

                    for (let i = 0; i < this.segments + 2; i++) {
                    this.anchors.push(0, 0);
                    this.radii.push(Math.random() * bumpRadius - halfBumpRadius);
                    this.thetaOff.push(Math.random() * 2 * Math.PI);
                    }

                    this.theta = 0;
                    this.thetaRamp = 0;
                    this.thetaRampDest = 12;
                    this.rampDamp = 25;
                }
                update() {
                    this.thetaRamp += (this.thetaRampDest - this.thetaRamp) / this.rampDamp;
                    this.theta += 0.03;
                    if(that.$data.expanding){
                        if(this.radius < 4000){
                            this.radius +=75;
                        }
                    }
                    this.anchors = [0, this.radius];
                    for (let i = 0; i <= this.segments + 2; i++) {
                    const sine = Math.sin(this.thetaOff[i] + this.theta + this.thetaRamp);
                    const rad = this.radius + this.radii[i] * sine;
                    const x = rad * Math.sin(this.step * i);
                    const y = rad * Math.cos(this.step * i);
                    this.anchors.push(x, y);
                    }

                    c.save();
                    c.translate(-10, -10);
                    c.scale(0.5, 0.5);
                    c.fillStyle = "rgb(32,34,37)";
                    c.beginPath();
                    c.moveTo(0, 0);
                    bezierSkin(this.anchors, false);
                    
                    c.lineTo(0, 0);
                    c.fill();
                    c.restore();
                }
                }

                const blob = new Blob();

                function loop() {
                c.clearRect(0, 0, canvas.width, canvas.height);
                blob.update();
                window.requestAnimationFrame(loop);
                }
                loop();

                // array of xy coords, closed boolean
                function bezierSkin(bez, closed = true) {
                const avg = calcAvgs(bez);
                const leng = bez.length;

                if (closed) {
                    c.moveTo(avg[0], avg[1]);
                    for (let i = 2; i < leng; i += 2) {
                    let n = i + 1;
                    c.quadraticCurveTo(bez[i], bez[n], avg[i], avg[n]);
                    }
                    c.quadraticCurveTo(bez[0], bez[1], avg[0], avg[1]);
                } else {
                    c.moveTo(bez[0], bez[1]);
                    c.lineTo(avg[0], avg[1]);
                    for (let i = 2; i < leng - 2; i += 2) {
                    let n = i + 1;
                    c.quadraticCurveTo(bez[i], bez[n], avg[i], avg[n]);
                    }
                    c.lineTo(bez[leng - 2], bez[leng - 1]);
                }
                }

                // create anchor points by averaging the control points
                function calcAvgs(p) {
                const avg = [];
                const leng = p.length;
                let prev;

                for (let i = 2; i < leng; i++) {
                    prev = i - 2;
                    avg.push((p[prev] + p[i]) / 2);
                }
                // close
                avg.push((p[0] + p[leng - 2]) / 2, (p[1] + p[leng - 1]) / 2);
                return avg;
                }
            },
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
                        this.$userState.loginShowName = res.showname;
                        this.$data.expanding = true;
                        document.getElementById("login-page").classList.add("hide");
                        setTimeout(()=>(this.request.SwitchPage()),2000)
                        
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
            },
            back : function(){
                this.isLogin = true;
            }
        },
        props :{
            request : Object
        }
    }
</script>

<style scoped lang="scss">
    input{
        font-size: 16px;
        color: var(--text-normal);
        border-radius: 3px;
        border: none;
        padding: 10px;
        background-color: var(--deprecated-text-input-bg);
    }
    html, body {
        width: 100%;
        height: 100%;
        margin: 0;
        padding: 0;
        user-select: none;
        
    }
    #root{
        width: 100%;
        height: 870px;
        text-align: left; 
        margin: auto;
        position: relative;
        background-image: url("../img/bg.png");
    }
    #root h1{
        text-align: center;
        color:white;
        font-weight: 600;

    }
    div{
        width: 300px;
        margin: auto;
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
        font-size: 12px;
        line-height: 16px;
        font-weight: 600;
        color: #8e9297;
    }

    .notice-item {
        position: relative;
        margin-top: 10px;
        margin-bottom: 10px;
        text-align: center;
        overflow: hidden;
        min-height: 32px;
        color: #8e9297;
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
    .block{
        position: absolute;
        width: 500px;
        margin-top: 200px;
        background-color: #36393f;
        padding: 32px;
        border-radius: 6px;
        box-shadow:0 2px 10px 0 rgba(0,0,0,.2);
        margin-left: 100px;
        z-index: 10;
        transition: opacity 1s ease-in-out;
    }
    .block.hide{
        opacity: 0;
    }
</style>