const express = require('express');
const app = express();

const PORT = 8080;

app.listen(PORT, () => 
{
    console.log('App started!');    
});

app.get('/test', (req, res) =>
{
    res.send('Hello');
});