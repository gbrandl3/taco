TACO::DBServer::DBServer()
{
//
// Find the dbm_database table names
//
    char 	*ptr;
    if ((ptr = (char *)getenv("DBTABLES")) == NULL)
    {
         cerr << "dbm_server: Can't find environment variable DBTABLES" << endl;
         exit(-1);
    }
//
// Automatically add a names and a ps_names tables
//
    dbgen.TblName.clear();
    dbgen.TblName.push_back("names");
    dbgen.TblName.push_back("ps_names");
    dbgen.ps_names_index = 1;
//
// Extract each table name
//
    string dbtables(ptr);
//
// Change database table names to lowercase letter names
//
    transform(dbtables.begin(), dbtables.end(), dbtables.begin(), tolower);
    string::size_type 	pos(0),
			start(0);

    while ((pos = dbtables.find(',', pos)) != string::npos)
    {
        string tmp_str(dbtables, start, pos - start);
        start = ++pos;
        if ((tmp_str == "names") || (tmp_str == "ps_names"))
             continue;
        dbgen.TblName.push_back(tmp_str);
    }
    dbgen.TblName.push_back(dbtables.substr(start));
    dbgen.TblNum = dbgen.TblName.size();
    return;
}
