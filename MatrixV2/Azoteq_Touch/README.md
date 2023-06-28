The board uses Azoteq chips for the touch (I did not get my custom touch working well enough - so was lazy and used the Azoteq.  Should move away from this, and just use a microcontroller...
Anyway.  You will need to program them. :-(  I would advise reading their manuals and adjusting the sensitivity better than I did..


Programming the touch needs to be done using an Azoteq CT-210 programmer (the one they specify for the Azoteq chip I am using)

- You can either use the hex or flash to program.
- You'll need to program via the header on the PCB (make a custom board - or use mine)
- Read the instructions to program in the Azoteq pdf: AZD087 – IQS5xx-B000 Setup and User Guide: https://www.azoteq.com/images/stories/pdf/AZD087%20-%20IQS5xx-B000%20Setup%20and%20User%20Guide.pdf

It would be a good idea to view the touch live via the I2C before programing to get a better idea as to what is going on with the touch.

