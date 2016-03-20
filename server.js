var express = require('express');
var morgan = require('morgan');
var bodyParser = require('body-parser');
var mysql = require('mysql');
var multiparty = require('multiparty');
var fs = require('fs');
var moment = require('moment');
var child_process = require('child_process');

var app = express();
var port = process.env.PORT || 3000;
app.use(morgan('dev'));
app.use(bodyParser.urlencoded({ extended : false}));
app.use(bodyParser.json());

/********************
* Support Functions *
********************/
function search_array(value, myArray) {
  for (var i = 0; i < myArray.length; i++) {
    if(value == myArray[i]) {
      return true;
    }
  }
}

/**********************
* Database Conenction *
**********************/
var connection = mysql.createConnection({
  host : 'librorum.in',
  user : 'arjhun',
  password : '$haringan1208!',
  database : 'c9'
});

// Inverse logic for check applies here
if(connection.connect()) {
    console.log('Database offline');
}

/*************
* API Routes *
*************/
app.get('/users', function(req,res) {
  sqlString = 'INSERT INTO users (name,android_id,active_flag) VALUES (?,?,?)';

  values = [];
  keys = Object.keys(req.query);

  values.push(req.query.name);
  values.push(req.query.android_id);
  values.push(req.query.active_flag);

  connection.query(sqlString,values,function(err,result) {
    if (err) {
      console.log(err);
    } else {
      res.send(result.insertId.toString());
    }
    // Handle unique exception
    sqlString = 'SELECT id FROM users WHERE android_id = ?';
    tmp = [];
    tmp.push(req.query.android_id);

    connection.query(sqlString,tmp,function(err,result) {
      if(err) {
        console.log(err);
      } else {
        res.send(result[0].id.toString());
      }
    });
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

app.get('/clusters', function (req,res,next) {
  // Middleware for handling clustering CPP file
  // Generate command for execution
  command = './main ' + req.query.user_id;
  console.log('Executing command : ' + command);
  child_process.exec(command, function(err,stdout,stderr) {
    if(err) {
      console.log('Execution quit with error code : ' + err.code);
    }
    console.log(stdout);
    path = req.query.user_id + '/ThumbNails';
    fs.chmodSync(path, 0770);
    next();
  });
}, function(req,res) {
  // SQL query
  sqlString = 'SELECT a.image_name,a.local_path,b.cluster_name,b.cluster_number FROM photos a,clusters b WHERE b.user_id = ? AND b.photo_id = a.id';

  // Populate values array
  values = [];
  values.push(req.query.user_id);

  connection.query(sqlString,values,function(err,result) {
    if(err) {
      console.log(err);
      res.send('Error');
    } else {
      // Generate final result
      final_result = {};
      final_result['images'] = result;
      res.send(final_result);
    }
  });
});

app.get('/thumbnails', function(req,res) {
  thumbnails = [];
  dir = req.query.user_id + '/ThumbNails';
  thumbs = fs.readdirSync(dir);
  thumbs.forEach(function(thumb) {
    tmp = {};
    values = [];
    sqlString = 'SELECT cluster_name FROM clusters WHERE cluster_number = ? LIMIT 1';

    tmp['cluster_number'] = thumb.split('.')[0];
    values.push(tmp['cluster_number']);
    connection.query(sqlString,values,function(err,result) {
      if(err) {
        console.log(err);
        res.send('Error');
      } else {
        res.send(result);
      }
    });
  });
});

app.get('/renameCluster', function(req,res) {
  // SQL query
  sqlString = 'UPDATE clusters SET cluster_name = ? WHERE cluster_number = ?';
  // Populate values array
  values = [];
  values.push(req.query.cluster_name);
  values.push(req.query.cluster_number);

  // Execute SQL query
  connection.query(sqlString,values,function(err,results) {
    if(err) {
      console.log(err);
      res.send('Error');
    } else {
      res.send('Success');
    }
  });
});

app.listen(port);
