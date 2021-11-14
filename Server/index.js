const express = require('express');

const app = express();
const port = 3000

app.use(express.json());

app.post('/', function (req, res) {
	console.log(req.body);
    res.sendStatus(200);
})

app.listen(port, () => {

	console.log('Hello World Running on localhost port:' + port);

})
