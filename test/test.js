// This is ugly, but there is no elegant solution (the EventEmitter code needs it)
globalThis.require = require;
const { EventEmitter } = require('events');
const { Base, Extended, MyEmitter } = require('../build/Release/napi-inherit-native.node');

console.log('CC inheritance:');
const b = new Base('Garga');
const ex = new Extended('Gargamel', 'Mr');

console.log('instanceof', ex instanceof Base);

console.log('base', b.greet());
console.log('extended', ex.greet());

console.log('inheritance', ex.mention());
console.log('');


console.log('EE inheritance:');
const eisenfunk = new MyEmitter();
console.log('instanceof', eisenfunk instanceof EventEmitter);
eisenfunk.on('data', (data) => {
  console.log(data);
});
eisenfunk.ping();
