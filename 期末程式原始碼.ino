#include <LiquidCrystal.h>
#include <IRremote.h>

const int IR_RECEIVE_PIN = 6;
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int removeValue;
int teamAscore = 0, teamBscore = 0;
int teamAwin = 0, teamBwin = 0, tempWin;
int currentTeam = 1;
int matchPoint=25; //25分制duece
IRrecv irrecv(IR_RECEIVE_PIN);
decode_results results;

const char* teamL = "A";
const char* teamR = "B";
int tempScore;
const char* tempTeam;
char buffer[100];
char buffer1[100];
// Declare these variables as global
bool buttonProcessed = false;
unsigned long lastButtonTime = 0;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.clear();
  updatescores();  // Initial display
  lcd.setCursor(0, 0);
  snprintf(buffer, sizeof(buffer), "  %s(%d)  :  %s(%d)", teamL, teamAwin, teamR, teamBwin);
  lcd.print(buffer);
  irrecv.enableIRIn();
}

void loop() {
  unsigned long currentMillis = millis();

  if (!end()) {
    if (irrecv.decode()) {
      removeValue = irrecv.decodedIRData.command;

      if (!buttonProcessed && (currentMillis - lastButtonTime > 100)) {
        handleIRButton();  // 處理紅外線遙控按鈕事件
      }

      irrecv.resume(); // 接收下一個紅外線信號
    } else {
      buttonProcessed = false;
    }
  }
}

void handleIRButton() {
  Serial.println(removeValue);
  switch (removeValue) {
    case 4:
      changeMatchPoint(25, "volleyball");
      break;
    case 5:
      changeMatchPoint(11, "table tennis");
      break;
    case 6:
      changeMatchPoint(21, "badminton");
      break;
    case 12:
      reset();
      break;
    case 16:
      teamAscore++;
      break;
    case 17:
      teamBscore++;
      break;
    case 18:
      if (teamAscore > 0)
        teamAscore--;
      break;
    case 20:
      if (teamBscore > 0)
        teamBscore--;
      break;
    case 21:
      swapTeams();  // 切換兩隊或交換勝利局數
      break;
    case 22:  // Example: Button 6 on the remote to increment Team A's win count
      teamAwin++;
      break;
    case 24:  // Example: Button 7 on the remote to increment Team B's win count
      teamBwin++;
      break;
    case 25:  // Example: Button 6 on the remote to increment Team A's win count
      if (teamAwin > 0)
        teamAwin--;
      break;
    case 26:  // Example: Button 7 on the remote to increment Team B's win count
      if (teamBwin > 0)
        teamBwin--;
      break;
    default:
      break;
  }

  updatescores();
  buttonProcessed = true;
  lastButtonTime = millis();

}

void changeMatchPoint(int newMatchPoint, const char* gameType) {
  lcd.clear();
  lcd.setCursor(0, 1);
  snprintf(buffer1, sizeof(buffer1), "%s-%d ", gameType, newMatchPoint);
  lcd.print(buffer1);

  delay(3000);
  lcd.clear();

  // Set the new match point
  matchPoint = newMatchPoint;

  lcd.setCursor(0, 0);
  snprintf(buffer, sizeof(buffer), "  %s(%d)  :  %s(%d)", teamL, teamAwin, teamR, teamBwin);
  lcd.print(buffer);
  updatescores();
}


void reset() {
  teamAwin = 0;
  teamBwin = 0;
  lcd.setCursor(0, 0);
  lcd.clear();
  teamAscore = 0;
  teamBscore = 0;
  snprintf(buffer, sizeof(buffer), "    %s(%d) :  %s(%d)", teamL, teamAwin, teamR, teamBwin);
  updatescores();
}


void swapTeams() {
  tempScore = teamAscore;
  teamAscore = teamBscore;
  teamBscore = tempScore;

  tempTeam = teamL;
  teamL = teamR;
  teamR = tempTeam;

  updatescores();

  // Additional functionality to swap win counts

  tempWin = teamAwin;
  teamAwin = teamBwin;
  teamBwin = tempWin;


  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  snprintf(buffer, sizeof(buffer), "   %s(%d) :  %s(%d)", teamL, teamAwin, teamR, teamBwin);
  lcd.print(buffer);
}

void updatescores() {
  lcd.setCursor(0, 1);
  lcd.print("              ");
  lcd.setCursor(0, 1);
  lcd.print("   ");
  snprintf(buffer1, sizeof(buffer), "%02d   :   %02d   ", teamAscore, teamBscore);
  lcd.print(buffer1);
  lcd.print("  ");  // 清空可能殘留的局數

  lcd.setCursor(0, 0);
  lcd.print("               ");
  lcd.setCursor(0, 0);
  snprintf(buffer, sizeof(buffer), "  %s(%d)  :  %s(%d)", teamL, teamAwin, teamR, teamBwin);
  lcd.print(buffer);
}


bool end() {
  lcd.setCursor(0, 0);
  if (teamAscore == matchPoint && teamBscore < matchPoint-1) {
    declareWinner("A");
    return true;
  } else if (teamBscore == matchPoint && teamAscore < matchPoint-1) {
    declareWinner("B");
    return true;
  } else if (teamAscore >= matchPoint-1 && teamBscore >= matchPoint-1) {
    if (teamAscore - teamBscore == 2) {
      declareWinner("A");
      return true;
    } else if (teamBscore - teamAscore == 2) {
      declareWinner("B");
      return true;
    }
    
  }
  return false;
}

void declareWinner(const char* winningTeam) {
  lcd.setCursor(0, 0);
  lcd.print("                ");

  // 保存勝利訊息以便還原
  char winMessage[17];  // 17是LCD的列寬
  snprintf(winMessage, sizeof(winMessage), "Team %s wins!", winningTeam);


  // 顯示勝利訊息
  lcd.setCursor(0, 0);
  lcd.print(winMessage);

  delay(3000);
  lcd.clear();
  if (strcmp(winningTeam, "A") == 0) {
    teamAwin++;
  } else {
    teamBwin++;
  }
  // 還原顯示
  lcd.clear();
  lcd.setCursor(0, 0);
  snprintf(buffer, sizeof(buffer), "  %s(%d) :  %s(%d)", teamL, teamAwin, teamR, teamBwin);
  lcd.print(buffer);

  teamAscore = 0;
  teamBscore = 0;
  updatescores();

}