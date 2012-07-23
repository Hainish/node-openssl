try {
    // node.js versions < 0.5.5
    var cc = new require('./build/default/openssl');
} catch(e) {
    // node.js versions >= 0.5.5
    var cc = new require('./build/Release/openssl');
}
var OpenSSL = cc.OpenSSL;

module.exports = OpenSSL;


