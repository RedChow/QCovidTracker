# QCovidTracker
 <h1>QCovid Tracker</h1>
    <h2>Introduction</h2>
    This is a personal project aimed at getting re-acquainted with Qt. I have developed around a dozen or so projects, but it has been several years since my last completed project.
    It is bare-bones at the moment, but there are many additions that I have planned.
    <h2>Usage</h2>
    After setting up the database tables and filling out the relevant information in databaseinterface.h, go to the tab "Get Data" and click on the button "Get Data." 
    This will start the process of downloading JSON files to parse and enter into the database.
    The JSON files are downloaded using the API from http://covidtracking.com.
    Because the JSON fields can change without notice, this program will automatically add new JSON fields into the database seamlessly.
    
  Currently the "Positive/Tests" tab show the positive number of positive test results along with the test numbers. 
    It defaults to show the differences between consecutive days, as this proves to typically more useful that the total accumulated.
    This is because differences show how fast outbreaks are happening.
    These graphs also allow the user to see how differences are stacking up against test numbers.
    <h3>Screenshots</h3>
    ![Alt text](/screenshots/getting_data.png?raw=true "Getting Data")
    
    
   <h2>Environment</h2>
        This was developed on Fedora 31 and openSUSE Tumbleweed using Qt Creator 4.12.3, Qt 5.14.2, and PostgreSQL 12.3.
    <h2>Database Configuration</h2>
        I went through a few database table designs before ultimately settling on the current. There were many differents ways of designing the tables and how they all interact, One design that I opposed is using column names as the JSON object keys. I've worked on systems with 250+ column names where each column had exactly one corresponding value in an entity. I cannot stress enough how awful it was working with said system. And since this project is supposed to be fun, that's a route I avoided at all costs.
        I originally though that the string fields in the dataset would be more interesting, however, in hindsight I could have left out the strng column all together.
        
   There are four main tables that are covered in the db_tables file. I originally planned on there possibly being more data_enum_types, but at the time of writing it seems that it'll probably be limited to integers and strings. 
        Because of this and the string fields being a bust, I have thought about redesigning completely.
        However, I've left the design as is, because, although it could be a little better, it works and could still be possibly useful in expanding certain tables.
        The file state_info_import is include to populate the state_info table.
        
   <h2>TODO & Future Plans</h2> 
        There are many plans for filling this program out to make it more complete and a better tool.
        I will have to add in these plans as I find time.
        <h3>Graphing</h3>
        Will probably add the ability that when the user clicks on a graph on the tab "Positive/Tests," the line stays highlighted.
        I will also add more graphs to "Other Plots," such as some stacked bar charts, bar and line graphs, and many others.
        <h3>Data Management</h3>
        Besides getting the JSON files from http://covidtracking.com, I plan on adding custom methods for entering data so that information from city, county, country, parish, or other can be added and graphed.
        I also plan on creating a UI to manually depcrate fields; this will cease storing information for those particular JSON fields.
        <h3>Other</h3>
        There are many TODO comments throughout. One in particular is in handlejsonfilesrunnable.cpp, in which several pieces of code could be refactored for better performance and readability.
        Would also like to add more comments to the "Get Data" tab that tells the user more about the process that is on-going.
        In the same vein, would also like to add logging into the program.
   <h2>Known Issues</h2>
