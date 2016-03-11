var express = require('express');
var morgan = require('morgan');
var bodyParser = require('body-parser');
var mysql = require('mysql');
var multiparty = require('multiparty');
var fs = require('fs');
var moment = require('moment');

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

if(!connection.connect()) {
    console.log('Database offline');
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

app.post('/upload', function(req,res) {
  var form = new multiparty.Form();
  dir = req.headers.user_id;

  form.parse(req, function(err,fields,files) {
    console.log(files);
    console.log(req.headers);
    res.send('success');

    // Check if upload was undefined
    if(typeof files !== 'undefined' && files) {
      // Generate new path for image
      newPath = './' + dir + '/' + files.upload[0].originalFilename;

      // Move file from tmp to local project
      if(!fs.existsSync(dir)) {
        fs.mkdirSync(dir,0766);
        fs.renameSync(files.upload[0].path, newPath);
      } else {
        fs.renameSync(files.upload[0].path, newPath);
      }

      // Insert into database
      sqlString = 'INSERT INTO photos (user_id,Image_name,Image_path,local_path,Image_time_stamp) VALUES (?,?,?,?,?)';

      // Populate values array
      values = [];
      values.push(dir);
      values.push(files.upload[0].originalFilename);
      values.push(newPath);
      values.push(req.headers.path);
      values.push((moment(Number(req.headers.timestamp)).format('YYYY-MM-DD HH:MM:SS')));
      //values.push(req.headers.timestamp);

      // Execute query
      connection.query(sqlString,values,function(err,result) {
        if(err) {
          console.log(err);
        }
      });
    }
  });
});

app.get('/clusters', function(req,res) {
  // SQL query
  sqlString = 'SELECT image_name,local_path FROM photos WHERE user_id = ?';

  // Populate values array
  values = [];
  values.push(req.query.user_id);

  connection.query(sqlString,values,function(err,result) {
    if(err) {
      console.log(err);
      res.send('Error');
    } else {
      res.send(result);
    }
  });
});

app.listen(port);
