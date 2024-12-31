# Fundamental Computer Vision without using OpenCV

The project was developed using QT Creator.  
Only the .cpp files are included, excluding the .h files.
<hr style="border-top: 3px solid #bbb;">

## Usage

<div style="display: flex; justify-content: center; align-items: center;">
  <img src="image1.png" alt="Image 1" style="width: 200px; height: 150px; margin-right: 10px;">
  <img src="image2.png" alt="Image 2" style="width: 200px; height: 150px; margin-right: 10px;">
  <img src="image3.png" alt="Image 3" style="width: 200px; height: 150px;">
</div>

## Data Collection Sources

- CBSSPORTS: https://www.cbssports.com/nba/scoreboard/
- CBSSPORTS: https://www.cbssports.com/nba/injuries/
- HOOPSHYPE: https://hoopshype.com/salaries/players/
- ESPN: https://www.espn.com/nba/stats/player
- Additionally, some data is retrieved using the nba_api library.
<hr style="border-top: 3px solid #bbb;">

## How to Use

- Sidebar:    
    Use the sidebar on the left to search for a player’s name and find their ID.

- Section 3. Player Shot Spot and Stat Analysis:  
    Enter the player's ID in the first input box to retrieve their shot attempt locations, successful shot locations, and stats.  
    If there is no data available for the player, graphs will not be displayed.  

- Section 4. Comparison of Win Rates and Shooting Percentages for 3 Players in the Last 5, 3, and 1 Minutes of a Game:  
    Enter the IDs of three players whose clutch-time performance you want to analyze.  
    This will display their win rates and shooting percentages during the last 5, 3, and 1 minutes of a game.  
    If there is insufficient clutch-time data, graphs may not appear.  
    It is recommended to input players with significant clutch-time data.  

- Section 9. Player Efficiency Scores and Salary Analysis:  
    Enter the player's ID in the input field to view information about their offensive/defensive efficiency scores and salary.  
<hr style="border-top: 3px solid #bbb;">

## Execution Image

![image](https://github.com/user-attachments/assets/d86aa8d9-4658-485c-bf6a-086ea0fa7faf)
![image](https://github.com/user-attachments/assets/cf0592d1-781d-4bfd-a70b-4de8aeb28c8b)
![image](https://github.com/user-attachments/assets/e88d5a8f-8288-44b1-af3d-ea9fde438e78)
![image](https://github.com/user-attachments/assets/c2a97d6b-2313-4ec1-80bd-0194d8ad3334)
![image](https://github.com/user-attachments/assets/711d01b4-9d0a-4d5e-baa2-6b0be7ce4b6a)
![image](https://github.com/user-attachments/assets/079244d2-376e-4c22-b2f7-cf6b0d57b63b)
<hr style="border-top: 3px solid #bbb;">

## Section 3 Results
![image](https://github.com/user-attachments/assets/710e8c85-4937-405d-bbf6-69eb74f75a02)
<hr style="border-top: 3px solid #bbb;">

## Section 4 Rssults
![image](https://github.com/user-attachments/assets/d0d0076b-3064-4305-a146-81845ac1b867)
<hr style="border-top: 3px solid #bbb;">

## Section 9 Results
![image](https://github.com/user-attachments/assets/0ec804c0-a66b-4c2a-9792-b846dc07544c)
![image](https://github.com/user-attachments/assets/6a84c735-a77b-431c-bfd2-cd18a7e97d7c)
<hr style="border-top: 3px solid #bbb;">

## Usage / Significance

Using information from Section 1, you can identify players currently in good form and prepare to defend against them strategically.  
By using the search bar in Section 3, analyze a player’s preferred shooting spots and details to create systematic game strategies.  
  
Using information from Section 2, you can identify the roster excluding injured players, which is useful for planning tailored strategies against the opposing team.  
  
Using information from Section 3, you can search specific players to analyze their preferred shooting spots, high-percentage shooting areas, and offensive patterns, enabling a customized defensive approach.  
  
Using information from Section 4, you can search specific players on the opposing team during clutch situations to identify who has the highest success rate and focus on defending that player.  
--> Additionally, you can search key players from each team in the final moments of a game to make predictions about the outcome.  
  
Using information from Section 5, you can access continuously updated data to monitor players’ abilities and performance.  
  
Using information from Section 6, you can assess the impact of players by position.  
--> This helps in player acquisitions or trades with limited budgets by identifying the most cost-effective positions in terms of offensive and defensive influence.  
  
Using information from Sections 7 and 8, you can evaluate the rankings and salaries of players performing well this season.  
--> This allows you to identify players undervalued in terms of salary relative to their abilities, making them ideal for trade or acquisition at a good price.  
  
Using information from Section 9, you can analyze players' efficiency and salary.  
--> When working within a restricted budget, this helps you identify players with the best cost-to-performance ratio for recruitment.  


