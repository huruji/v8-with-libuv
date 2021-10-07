console.log(123)
console.error(123)

console.log("before setTimeout")

setTimeout(() => {
console.log('in setTimeout')
}, 5000);

console.log('after setTimeout')
// console.log(a)
// console.log(a.start())
