#include </usr/include/mysql++/mysql++.h>
#include "/usr/include/mysql++/cmdline.h"
#include <iostream>
#include <iomanip>

using namespace std;

/* global declarations here */
mysqlpp::Connection conn(false);

/*This Function establishes connection with the database */

void EstablishConnection(const char* db , const char* host , const char* user , const char* pass)
{
	if(conn.connect(db,host,user,pass))
		cout << "Established Connection" << endl;
	else
		cout << "Unable to Establish Connection" << conn.error() << "\n";
}

/* This function verifies user and feteches the image_array */

vector<string> FetchData(int id)
{
	vector<string> images;
	stringstream query_string;
    query_string << "SELECT * FROM `users` WHERE `id` = " << id ;
    string qs = query_string.str();
	mysqlpp::Query query = conn.query(qs);
	if(mysqlpp::StoreQueryResult res = query.store())
	{
		if(res.num_rows() == 1)
		{
			cout << "Valid User.. Fetching Image Details" << endl;
			stringstream query_string_2;
			query_string_2 << "SELECT * FROM `photos` WHERE `user_id` = " << id << " AND `processed` = " << 0 ;
			qs = query_string_2.str();
			query = conn.query(qs);
			if(mysqlpp::StoreQueryResult result = query.store())
			{
				for(int i = 0 ; i < result.num_rows() ; i++)
				{
					stringstream tmp;
					tmp << result[i][3];
					string s = tmp.str();
					images.push_back(s);
				}

			}
			else
			{
				cout << "Error : " << query.error() << "\n";
				exit(1);
			}
		}

	}
	else
	{
		cout << "Error :" << query.error();
		exit(1);
	}
	return images;
}

/* This function returns the image_id */
vector<int> FetchID(int id)
{
	vector<int> ids;
	stringstream query_string;
    query_string << "SELECT * FROM `users` WHERE `id` = " << id ;
    string qs = query_string.str();
	mysqlpp::Query query = conn.query(qs);
	if(mysqlpp::StoreQueryResult res = query.store())
	{
		if(res.num_rows() == 1)
		{
			cout << "Valid User.. Fetching ID" << endl;
			stringstream query_string_2;
			query_string_2 << "SELECT * FROM `photos` WHERE `user_id` = " << id << " AND `processed` = " << 0 ;
			qs = query_string_2.str();
			query = conn.query(qs);
			if(mysqlpp::StoreQueryResult result = query.store())
			{
				for(int i = 0 ; i < result.num_rows() ; i++)
				{
					int si = result[i][0];
					ids.push_back(si);
				}

			}
			else
			{
				cout << "Error : " << query.error() << "\n";
				exit(1);
			}
		}
	}
	else
	{
		cout << "Error :" << query.error();
		exit(1);
	}
	return ids;
}



/*This function populates the cluster table */

void PopulateCluster(string cluster_number,int pid , int uid , string path)
{
	//stringstream select_string;
	//string qs = select_string.str();
	//mysqlpp::Query query = conn.query(qs);
	//if(mysqlpp::StoreQueryResult res = query.store())
	//{
	//	if(res.num_rows() == 0)
	//	{
			cout << "Inserted" << "\n";
			stringstream query_string;
    		query_string << "INSERT into `clusters`(`user_id`,`photo_id`,`cluster_number`,`filtered_image_path`) values("<< uid << "," << pid << "," << cluster_number << ",'" << path <<  "')" ;
    		string qs = query_string.str();
			mysqlpp::Query query1 = conn.query(qs);
			query1.store();
	//	}
	//	else
	//	{
	//		cout << "Updated" << "\n";
	//		stringstream query_string;
    //		query_string << "UPDATE `clusters` SET `cluster_number` = " << cluster_number << " , SET `filtered_image_path` = " << path << " WHERE `user_id` = " << uid << " AND `photo_id` = " << pid ;
    //		qs = query_string.str();
    //		cout <<  qs;
	//		mysqlpp::Query query1 = conn.query(qs);
	//		query1.store();
	//	}

	//}
}
