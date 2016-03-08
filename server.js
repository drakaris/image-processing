var express = require('express');
var morgan = require('morgan');
var bodyParser = require('body-parser');
var mysql = require('mysql');
var multiparty = require('multiparty');

var app = express();
var port = process.env.PORT || 3000;
app.use(morgan('dev'));
app.use(bodyParser.urlencoded({ extended : false}));
app.use(bodyParser.json());

var connection = mysql.createConnection({
  host : 'localhost',
  user : 'root',
  password : '$haringan1208!',
  database : 'c9'
});

/*
if(!connection.connect()) {
    //console.log('Database offline');
}

app.get('/users', function(req,res) {
  sqlString = 'INSERT INTO users (name,android_id,active_flag) VALUES (?,?,?)';

  values = [];
  keys = Object.keys(req.query);

  keys.forEach(function(item) {
    values.push(req.query[item]);
  });

  connection.query(sqlString,values,function(err,result) {
    if (err) {
      console.log(err);
    } else {
      res.send(result.insertId.toString());
    }
    res.send('UNIQUE exception');
  });
});
*/

app.get('/test',function(req,res) {
  res.send(
    '<form action="/upload" enctype="multipart/form-data" method="post">'+
    '<input type="text" name="title"><br>'+
    '<input type="file" name="upload" multiple="multiple"><br>'+
    '<input type="submit" value="Upload">'+
    '</form>'
  );
});

app.post('/upload', function(req,res) {
  var form = new multiparty.Form();

  form.parse(req, function(err,fields,files) {
    console.log(files);
    res.send('True');
  });
});

app.listen(port);
