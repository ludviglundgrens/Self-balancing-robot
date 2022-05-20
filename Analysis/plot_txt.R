setwd("~/Desktop/KEX_Mekatronik/Analysis")
data <- read.csv("impulssvar2.txt", skip = 1)

library(tidyverse)
library(zoo)

# Get loop time
data %>% slice(500:1300) %>% 
  mutate(Time = Time - lag(Time, 1)) %>%
  filter(abs(Time) < 30000)  %>% drop_na() %>% summarize(mean = mean(Time))

# Get loop time (old version)
olddata <- read.csv("data.txt", skip = 1)
colnames(olddata) <- c("Time", "Vals1", "Vals2", "Vals3")
olddata %>% 
  mutate(Time = Time - lag(Time, 1)) %>%
  filter(abs(Time) < 30000)  %>% 
  drop_na() %>% 
  summarize(mean = mean(Time))

data %>% slice(500:1300) %>% 
  mutate(Time = Time - lag(Time, 1)) %>%
  filter(abs(Time) < 30000) %>%
  mutate(Time = cumsum(Time)) %>%
  mutate(Theta = Theta/50)-> impuls

plot(impuls$Time/1000, impuls$Theta, type = "l", xlab = "Time (milliseconds)", ylab="Angle (degrees)")


data %>% slice(3000:3800) %>%
  mutate(Time = Time - lag(Time, 1)) %>%
  filter(abs(Time) < 30000) %>%
  mutate(Time = cumsum(Time)) %>%
  mutate(Theta = Theta/50)-> ocillation

plot(ocillation$Theta , type = "l", xlab = "Time (milliseconds)", ylab="Angle (degrees)")


