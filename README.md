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
    Positivity rates have also been added to the Positive/Tests tab.
    The plot points are calculate by dividing the number of new positive cases by the number of new positive and negative tests.
    Positivity rate plot can be turned off and on.
    <h3>Screenshots</h3>
    ![Alt text](/screenshots/getting_data.png?raw=true "Getting Data")
    
   ![Alt text](/screenshots/AZ_positive_tests.png?raw=true "AZ Positive/Tests")
   The screenshot shows default graphs.

  ![Alt_text](/screenshots/AZ_positivity_rate.png?raw=true "Manually Add Data")    
  Positivity rate can be turned off and on. The above graph shows the positivity rate for AZ.
   
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
  <br>
  ![Alt_text](/screenshots/tx_custom_plot_config.png?raw=true "TX Custom Plots")    
  The above screen shot shows configuring custom plots.  Any state, whether it's from Covid Tracking Project or manually added can have custom plots made and saved. 
  This screen shot shows a custom made plot showing the hospitalizedCurrently, hospitalizedCurrently moving average, and deaths for the state of TX.
  Any field that has data can be selected and shown. The options for graphs are either line or bar, although there are plans to eventually added a stacked bar chart and possibly box and whisker plots.
  Style of the graph can be configured as well as the color of the graphs.  The Y-Axis ID allows plots to be shown on the same set of y-axis values or it's possible to select a different set.
  The plot below shows this plot.  
  Note the line and bar graphs are on separate Y-axes.
  <br>
![Alt_text](/screenshots/tx_custom_plot.png?raw=true "TX Custom Plots")    
  <br>
 ![Alt_text](/screenshots/midland_custom_plot_config.png?raw=true "Midland Custom Plots")     
  The plot configure above shows a city that is not tracked on the Covid Tracking Project. The data for this entity is manually added. The plot below shows this custom plot.
![Alt_text](/screenshots/midland_custom_plot.png?raw=true "Midland Custom Plots")     

  
    
   <h2>Environment</h2>
        This was developed on Fedora 31 and openSUSE Tumbleweed using Qt Creator 4.12.3, Qt 5.14.2, and PostgreSQL 12.3.
    <h2>Database Configuration</h2>
        I went through a few database table designs before ultimately settling on the current. There were many differents ways of designing the tables and how they all interact, One design that I opposed is using column names as the JSON object keys. I've worked on systems with 250+ column names where each column had exactly one corresponding value in an entity. I cannot stress enough how awful it was working with said system. And since this project is supposed to be fun, that's a route I avoided at all costs.
        I originally though that the string fields in the dataset would be more interesting, however, in hindsight I could have left out the strng column all together.
        
   There are four main tables that are covered in the db_tables file. I originally planned on there possibly being more data_enum_types, but at the time of writing it seems that it'll probably be limited to integers and strings. 
        Because of this and the string fields being a bust, I have thought about redesigning completely.
        However, I've left the design as is, because, although it could be a little better, it works and could still be possibly useful in expanding certain tables.
        The file state_info_import is include to populate the state_info table.
        
	
<i>Update 2020-09-29</i>: I have scrapped parsing the JSON fields and having an entry for each field in the table covid_history. I have instead opted for adding the table covid_history_json, which stores the history in JSON files. I will update the project with my Python web scrapers that automatically go and get information periodically through scheduled cron jobs. Storing the information is JSON files directly seems to be working better since parsing every single field isn't necessary. Plus, the data varies greatly different across the many different levels of government (city, county, state, hospital region, etc). This type of situation is exactly what the JSON column in Postgres was designed to handle.

This also signifies the need for a data abstraction layer between the database and the program. A major change in the database structure is going to need a lot of work on the GUI and internals of the main program. Had I not been in such a hurry to get this done, I would have taken the time to completely unhinge the two. So, life lesson, take the time to do it right the first time can may take more time initially, but can save time down the road.
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
