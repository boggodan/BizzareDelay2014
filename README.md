BizzareDelay2014
================

The old BizzareDelay project has trouble compiling because it's 3 years old and the SDK has changed. This is a new refactored version 
that will compile properly with the new IPlug version. 

I have reconstructed the entire project from scratch by cloning IPlugEffect and copying in the old source files. 

BizzareDelay VST/AU Plug-in.

This is a plug-in developed for BizzareContact, a psytrance project from Israel. It is now completely free. 
https://www.facebook.com/BizzareContactMusic

You can find the old compiled binaries for OSX and Windows here:
http://www.vst4free.com/free_vst.php?plugin=Bizzare_Delay&id=1404

I developed this in 2011, so it's quite old and there are many things that could be improved, however I quite like the plug-in
and have been using it as my main delay plug-in for my music. 

It simulates two delay racks in one: the top one is a straightforward stereo delay with independent controls for each delay. The top one
is designed to Bizzare's specification and it's a saturating tape delay that allows real time control of the delay time on a fine level
for weird psychedelic effects. Even though it's possible to change the delay on a fine level, the fine control still snaps to a full
tempo unit halfway along its range, so it's tempo synced if required.

It is written using Oli Larkin's WDL-OL framework (based on Cockos's IPlug). You need to set it up properly to compile, which is not difficult.

Credits: 
Bogdan Vera - Code, Design
Matt O'Neill - Graphics, Business

***HOW TO COMPILE***

You need to set-up WLD-OL as described by Oli Larkin:
https://github.com/olilarkin/wdl-ol

The BizzareDelay project folder is meant to sit somewhere inside the wdl-ol folder, or you can put it somewhere else but make sure you set up your include folders properly. 

You need Visual C++ Express 2010 to compile on Windows, and Xcode 4 or 5 to compile on OSX. You may need to fiddle with the XCode settings (read Oli's posts on his github).
