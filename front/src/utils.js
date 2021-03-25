var delayPromise = function(ms){
    return new Promise(function(resolve){
        setTimeout(resolve,ms);
    })
}

exports.timeoutPromise = function(promise,ms) {
    let timeout = delayPromise(ms).then(function(){
        throw new Error('time out.');
    });
    return Promise.race([promise,timeout]);
}