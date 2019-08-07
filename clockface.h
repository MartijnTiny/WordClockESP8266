#include "led_layout.h"

void pushToStrip(uint8_t Led)
{
  strip_ptr[stackptr] = Led;
  stackptr++;
}

///////////////////////
//PUSH WORD HELPER
///////////////////////
void W_LOGO()  {
  pushToStrip(L2);
}


void W_FEEST()  {
  pushToStrip(L95);
  pushToStrip(L84);
  pushToStrip(L73);
  pushToStrip(L62);
  pushToStrip(L51);
}

void W_PARTY()  {
  pushToStrip(L57);
  pushToStrip(L58);
  pushToStrip(L59);
  pushToStrip(L60);
  pushToStrip(L61);
}

void W_MODE()  {
  pushToStrip(L55);
  pushToStrip(L54);
  pushToStrip(L53);
  pushToStrip(L52);
}

void M_HETIS()  {
  pushToStrip(L122);
  pushToStrip(L121);
  pushToStrip(L120);
  pushToStrip(L118);
  pushToStrip(L117);
}

void M_VIJF() {
pushToStrip(L115);
pushToStrip(L114);
pushToStrip(L113);
pushToStrip(L112);
}

void M_OVER() {
pushToStrip(L99);
pushToStrip(L98);
pushToStrip(L97);
pushToStrip(L96);
}

void M_TIEN() {
pushToStrip(L101);
pushToStrip(L102);
pushToStrip(L103);
pushToStrip(L104);
}

void M_KWART() {
pushToStrip(L106);
pushToStrip(L107);
pushToStrip(L108);
pushToStrip(L109);
pushToStrip(L110);
}

void M_VOOR() {
pushToStrip(L93);
pushToStrip(L92);
pushToStrip(L91);
pushToStrip(L90);
}

void M_HALF() {
pushToStrip(L79);
pushToStrip(L80);
pushToStrip(L81);
pushToStrip(L82);
}

void M_UUR() {
pushToStrip(L5);
pushToStrip(L4);
pushToStrip(L3);
}

void M_ONE() {
pushToStrip(L123);
}

void M_TWO() {
pushToStrip(L124);
}

void M_THREE() {
pushToStrip(L125);
}

void M_FOUR() {
pushToStrip(L126);
}

void H_EEN() {
pushToStrip(L27);
pushToStrip(L26);
pushToStrip(L25);
}

void H_TWEE() {
pushToStrip(L63);
pushToStrip(L64);
pushToStrip(L65);
pushToStrip(L66);
}

void H_DRIE() {
pushToStrip(L19);
pushToStrip(L20);
pushToStrip(L21);
pushToStrip(L22);
}

void H_VIER() {
pushToStrip(L10);
pushToStrip(L9);
pushToStrip(L8);
pushToStrip(L7);
}

void H_VIJF() {
pushToStrip(L14);
pushToStrip(L15);
pushToStrip(L16);
pushToStrip(L17);
}

void H_ZES() {
pushToStrip(L70);
pushToStrip(L69);
pushToStrip(L68);
}

void H_ZEVEN() {
pushToStrip(L35);
pushToStrip(L36);
pushToStrip(L37);
pushToStrip(L38);
pushToStrip(L39);
}

void H_ACHT() {
pushToStrip(L32);
pushToStrip(L31);
pushToStrip(L30);
pushToStrip(L29);
}

void H_NEGEN() {
pushToStrip(L76);
pushToStrip(L75);
pushToStrip(L74);
pushToStrip(L73);
pushToStrip(L62);
}

void H_TIEN() {
pushToStrip(L42);
pushToStrip(L43);
pushToStrip(L44);
pushToStrip(L45);
}

void H_ELF() {
pushToStrip(L84);
pushToStrip(L85);
pushToStrip(L86);
}

void H_TWAALF() {
pushToStrip(L51);
pushToStrip(L50);
pushToStrip(L49);
pushToStrip(L48);
pushToStrip(L47);
pushToStrip(L46);
}

///////////////////////

void timeToStrip(uint8_t hours,uint8_t minutes)
{
  M_HETIS();

  //show minutes
  if(minutes >= 5 && minutes < 10) {
    M_VIJF();
    M_OVER();
  } else if(minutes >= 10 && minutes < 15) {
    M_TIEN();
    M_OVER();
  } else if(minutes >= 15 && minutes < 20) {
    M_KWART();
    M_OVER();
  } else if(minutes >= 20 && minutes < 25) {
      M_TIEN();
      M_VOOR();
      M_HALF();
  } else if(minutes >= 25 && minutes < 30) {
    M_VIJF();
    M_VOOR();
    M_HALF();
  } else if(minutes >= 30 && minutes < 35) {
    M_HALF();
  } else if(minutes >= 35 && minutes < 40) {
    M_VIJF();
    M_OVER();
    M_HALF();
  } else if(minutes >= 40 && minutes < 45) {
      M_TIEN();
      M_OVER();
      M_HALF();
  } else if(minutes >= 45 && minutes < 50) {
    M_KWART();
    M_VOOR();
  } else if(minutes >= 50 && minutes < 55) {
    M_TIEN();
    M_VOOR();
  } else if(minutes >= 55 && minutes < 60) {
    M_VIJF();
    M_VOOR();
  }
  
  int singleMinutes = minutes % 5;
  switch(singleMinutes) {
    case 1:
      M_ONE();
      break;
    case 2:
      M_ONE();
      M_TWO();
      break;
    case 3:
      M_ONE();
      M_TWO();
      M_THREE();
      break;
    case 4:
      M_ONE();
      M_TWO();
      M_THREE();
      M_FOUR();
    break;
  }

  if(hours >= 12) {
    hours -= 12;
  }
  if(minutes >= 20) {
      hours++;
  }

  if(hours == 12) {
    hours = 0;
  }

  //show hours
  switch(hours) {
    case 0:
      H_TWAALF();
      break;
    case 1:
      H_EEN();
      break;
    case 2:
      H_TWEE();
      break;
    case 3:
      H_DRIE();
      break;
    case 4:
      H_VIER();
      break;
    case 5:
      H_VIJF();
      break;
    case 6:
      H_ZES();
      break;
    case 7:
      H_ZEVEN();
      break;
    case 8:
      H_ACHT();
      break;
    case 9:
      H_NEGEN();
      break;
    case 10:
      H_TIEN();
      break;
    case 11:
      H_ELF();
      break;
  }
  
  //show hour
  if(minutes < 5) {
    M_UUR();
  }
}
