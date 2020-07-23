# QCovidTracker

   <h2>Introduction</h2>
    This is a personal project aimed at getting re-acquainted with Qt. I have developed around a dozen or so projects, but it has been several years since my last completed project.
    It is bare-bones at the moment, but there are many additions that I have planned.
    <h2>Usage</h2>
    After setting up the database tables and filling out the relevant information in databaseinterface.h, go to the tab "Get Data" and click on the button "Get Data." 
    This will start the process of downloading JSON files to parse and enter into the database.
    The JSON files are downloaded using the API from http://covidtracking.com.
    Because the JSON fields can change without notice, this program will automatically add new JSON fields into the database seamlessly.
    
  Currently the "Positive/Tests" tab show the positive number of positive test results along with the test numbers. 
    It defaults to show the differences between consecutive days, as this proves to typically more useful than the total accumulated.
    This is because differences show how fast outbreaks are happening.
    These graphs also allow the user to see how differences are stacking up against test numbers.
    <h3>Screenshots</h3>
    ![Alt text](/screenshots/getting_data.png?raw=true "Getting Data")
    
   ![Alt text](/screenshots/AZ_positive_tests.png?raw=true "AZ Positive/Tests")
   The screenshot shows default graphs.
   
  ![Alt text](/screenshots/AZ_raw_data.png?raw=true "Raw Data")
  Graphs on "Positive/Tests" tab are constructed from the raw data table. The raw data table also shows slope calculations over the moving averages.
  
  ![Alt text](/screenshots/AZ_tests_moving_avg_highlight.png?raw=true "Mouse Highlights graph")
  Moving mouse over the graphs will highlight them. Also shows data for the day in mouseover text box.
  
  ![Alt text](/screenshots/AZ_tracing.png?raw=true "Tracing")
  Right-clicking will be up a context menu in which the user can select/deselect a "Tracing" option.

  ![Alt_text](/screenshots/json_field_editor.png?raw=true "JSON/History editor")
  JSON and History field editor. Set fields to deprecated if no longer want to keep history for that field. There is not delete option (yet) since the program will automatically add in fields it parses from the Covid Tracking Project.
    
  ![Alt_text](/screenshots/phoenix_ax_manually_add_data.png?raw=true "Manually Add Data")    
  Added table model and item delegates for adding manual sources. Note that in the above screenshot, the data column fields are not from the Covid Tracking Project, but custom data fields.
  <br>
   There also exist options for zooming in and out using the mouse, using the arrow keys, and by using A, W, S, and D keys (not case-sensitive).

    
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
	<ul>
        	<li>Will probably add the ability that when the user clicks on a graph on the tab "Positive/Tests," the line stays highlighted.
        I will also add more graphs to "Other Plots," such as some stacked bar charts, bar and line graphs, and many others.</li>
        <ul>
        		<li><i>Update 2020-07-22: Scrapped the "Other Plots" and added "Custom Plots." My plans for "Other Plots" would not work in general since the data that the states keep outside of positive and
        		negative tests varies too greatly.</i></li>
        </ul>
	</ul>
        <h3>Data Management</h3>
	<ul>
        	<li>Besides getting the JSON files from http://covidtracking.com, I plan on adding custom methods for entering data so that information from city, county, country, parish, or other can be added and graphed.</li>
		<ul>
			<li><i>Update 2020-07-11: Added table models and item delegates for adding own manual data sources. No more spreadsheets for me!</i></li>
		</ul>
        	<li>I also plan on creating a UI to manually depcrate fields; this will cease storing information for those particular JSON fields.</li>
		<ul>
			<li><i>Update 2020-07-08: Have several dialogs, custom QItemDelegates and QAbstractTableModels, and database changes in the works.</i></li>
		</ul>
	</ul>
        <h3>Other</h3>
        There are many TODO comments throughout.
	<ul>
		<li> One in particular is in handlejsonfilesrunnable.cpp, in which several pieces of code could be refactored for better performance and readability. </li>
		<ul>
			<li><i>Update 2020-07-08: Made a better SQL statement for getting info from the Covid Tracking Project. </i></li>
		</ul>
        	<li>Would also like to add more comments to the "Get Data" tab that tells the user more about the process that is on-going.</li>
        	<li>In the same vein, would also like to add logging into the program.</li>
		<li>Code needs clean up in several areas, namely need to add an abstraction layer between table models and database interface.  This will make it easier in the future in many scenarios. For example,  if database needs changing (different database usually means revising SQL statements), or if we choose to store data in flat text files, spreadsheets, etc, we don't have to hunt down all the SQL, change or rewrite, etc.</li>
		<li>Need to add comments to many places in the code base.</li>
		<li>Need to add consistency in variable naming and point usage.</li>
		<li>Comb through code for memory leaks.</li>
	</ul>
   <h2>Known Issues</h2>
