<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.11.2" name="wiztiles" tilewidth="16" tileheight="16" tilecount="15" columns="0">
 <grid orientation="orthogonal" width="1" height="1"/>
 <tile id="0">
  <image source="sprites/blocks2/halo.png" width="16" height="16"/>
  <objectgroup draworder="index" id="2">
   <object id="1" x="0" y="0" width="16" height="16"/>
   <object id="2" x="0" y="0" width="16" height="16"/>
  </objectgroup>
 </tile>
 <tile id="1">
  <image source="sprites/blocks2/halot.png" width="16" height="16"/>
  <objectgroup draworder="index" id="2">
   <object id="1" x="0" y="0" width="16" height="16"/>
  </objectgroup>
 </tile>
 <tile id="2" type="slope-up">
  <properties>
   <property name="uplift" type="int" value="1"/>
  </properties>
  <image source="sprites/blocks2/halotilt.png" width="16" height="16"/>
  <objectgroup draworder="index" id="2">
   <object id="13" x="6" y="10.25" width="1.625" height="5.625"/>
   <object id="15" x="7.3125" y="7.875" width="8.5625" height="2.25"/>
   <object id="16" x="12.9375" y="2.375" width="2.9375" height="5.4375"/>
   <object id="17" x="2.125" y="14.125" width="3.75" height="1.75"/>
   <object id="18" name="slope-up" x="7.75" y="10.25" width="8.125" height="5.625"/>
  </objectgroup>
 </tile>
 <tile id="3" type="slope-down">
  <properties>
   <property name="uplift" type="int" value="1"/>
  </properties>
  <image source="sprites/blocks2/halotilt2.png" width="16" height="16"/>
  <objectgroup draworder="index" id="2">
   <object id="1" x="0.0625" y="4.5">
    <polygon points="0,0 9.5,11.5 -0.0625,11.4375"/>
   </object>
  </objectgroup>
 </tile>
 <tile id="4">
  <image source="sprites/blocks2/p.png" width="16" height="16"/>
  <objectgroup draworder="index" id="2">
   <object id="1" x="0" y="0" width="16" height="16"/>
  </objectgroup>
 </tile>
 <tile id="5">
  <image source="sprites/blocks2/pt.png" width="16" height="16"/>
  <objectgroup draworder="index" id="2">
   <object id="1" x="0" y="0" width="16" height="16"/>
  </objectgroup>
 </tile>
 <tile id="6">
  <image source="sprites/blocks2/b.png" width="16" height="16"/>
  <objectgroup draworder="index" id="2">
   <object id="1" x="0" y="0" width="16" height="16"/>
  </objectgroup>
 </tile>
 <tile id="7">
  <image source="sprites/blocks2/tiltl.png" width="16" height="16"/>
  <objectgroup draworder="index" id="2">
   <object id="1" x="6" y="10" width="10" height="6"/>
   <object id="2" x="11.125" y="5" width="4.8125" height="4.9375"/>
  </objectgroup>
 </tile>
 <tile id="8">
  <image source="sprites/blocks2/tiltr.png" width="16" height="16"/>
  <objectgroup draworder="index" id="2">
   <object id="1" x="-0.125" y="10" width="10.125" height="5.875"/>
   <object id="2" x="0.0625" y="4.875" width="5.75" height="5.125"/>
  </objectgroup>
 </tile>
 <tile id="9">
  <image source="sprites/blocks2/wall.png" width="16" height="16"/>
  <objectgroup draworder="index" id="2">
   <object id="1" x="9" y="0" width="7" height="16"/>
  </objectgroup>
 </tile>
 <tile id="10">
  <image source="sprites/blocks2/wall2.png" width="16" height="16"/>
  <objectgroup draworder="index" id="2">
   <object id="1" x="0" y="0" width="7" height="16"/>
  </objectgroup>
 </tile>
 <tile id="11">
  <image source="sprites/blocks2/ceiling.png" width="16" height="16"/>
  <objectgroup draworder="index" id="2">
   <object id="1" x="0" y="0" width="16" height="7"/>
  </objectgroup>
 </tile>
 <tile id="12">
  <image source="sprites/blocks2/floor.png" width="16" height="16"/>
  <objectgroup draworder="index" id="2">
   <object id="1" x="0" y="9" width="16" height="7"/>
  </objectgroup>
 </tile>
 <tile id="13">
  <image source="sprites/blocks2/bloop.png" width="16" height="16"/>
  <objectgroup draworder="index" id="2">
   <object id="1" x="12" y="12" width="4" height="4"/>
   <object id="2" x="12" y="12" width="4" height="4"/>
  </objectgroup>
 </tile>
 <tile id="14">
  <image source="sprites/blocks2/bloopt.png" width="16" height="16"/>
  <objectgroup draworder="index" id="2">
   <object id="1" x="0" y="12" width="4" height="4"/>
   <object id="2" x="0" y="12" width="4" height="4"/>
  </objectgroup>
 </tile>
</tileset>
