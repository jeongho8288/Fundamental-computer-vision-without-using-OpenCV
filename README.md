# Fundamental Computer Vision without using OpenCV

The project was developed using QT Creator.  
Only the .cpp files are included, excluding the .h files.
<hr style="border-top: 3px solid #bbb;">

## Conversion of RGB Image to Gray, HSI, and NRG Images  

RGB to Gray:  
<img src="https://github.com/user-attachments/assets/17179403-ac3f-4d7b-a65c-3dfd2bc11ad3" alt="Description" style="width: 60%;">  
  
RGB to HSI:  
<img src="https://github.com/user-attachments/assets/594717d6-2db7-474f-83b5-54a3863d039b" alt="Description" style="width: 60%;">  

RGB to NRG:  
<img src="https://github.com/user-attachments/assets/9dd35080-998b-44bf-8b31-d8d6f4e74750" alt="Description"  style="width: 60%;">   
<hr style="border-top: 3px solid #bbb;">

## Luminance Contrast Transform

<div style="display: flex; justify-content: space-around; align-items: center;">
  <img src="https://github.com/user-attachments/assets/e2484b22-11cd-4c9f-8706-94e1197ef195" alt="Description" style="width: 30%; margin-right: 2%;">
  <img src="https://github.com/user-attachments/assets/a1b08101-c8d4-4c08-9310-ea00a0edfbef" alt="Description" style="width: 30%;">
</div>
<hr style="border-top: 3px solid #bbb;">

## Dilation, Erosion Morphology

The two images above are the dilation and erosion images with a 3x3 kernel applied.  
The two images below are the dilation and erosion images with a 5x5 kernel applied.  
<div style="display: flex; justify-content: space-around; align-items: center;">
  <img src="https://github.com/user-attachments/assets/c8901c4d-aff7-4ef1-96cb-7ba043bd342f" alt="Description" style="width: 30%; margin-right: 2%;">
  <img src="https://github.com/user-attachments/assets/663c860f-8fe7-4436-a54d-39f0aac396a9" alt="Description" style="width: 30%;">
</div>
<hr style="border-top: 3px solid #bbb;">

## Labeling

Using 4-neighbor method on the images obtained by applying dilation and erosion with a 3x3 kernel:  
<div style="display: flex; justify-content: space-around; align-items: center;">
  <img src="https://github.com/user-attachments/assets/d1dcd7fa-0162-4996-9cd6-faf7cf31f83d" alt="Description" style="width: 40%; margin-right: 2%;">
  <img src="https://github.com/user-attachments/assets/ddefbafa-4b3e-4b81-b9cb-d458c0e5efce" alt="Description" style="width: 20%;">
</div>

Using 4-neighbor method on the images obtained by applying dilation and erosion with a 5x5 kernel:  
<div style="display: flex; justify-content: space-around; align-items: center;">
  <img src="https://github.com/user-attachments/assets/52147598-730a-4846-ae00-d2de4b1cc6fd" alt="Description" style="width: 40%; margin-right: 2%;">
  <img src="https://github.com/user-attachments/assets/4101562e-1731-4a3c-bf39-5870731e35e7" alt="Description" style="width: 20%;">
</div>
<hr style="border-top: 3px solid #bbb;">

## Opening & Closing Image

- Opening & Closing with 3x3 kernel  
<img src="https://github.com/user-attachments/assets/64863e12-41f7-409e-ae1b-05391c4224af" alt="Description" style="width: 60%;">  
  
- Opening & Closing with 5x5 kernel  
<img src="https://github.com/user-attachments/assets/78eae748-a8ba-4ab8-add7-6a731da87a65" alt="Description" style="width: 60%;">  
<hr style="border-top: 3px solid #bbb;">

## Boundary Extraction Image

- Foreground & Backgroung Image ( Type : Mb4 )  
<img src="https://github.com/user-attachments/assets/323f2e09-201b-44bb-aebd-c6bf8ef09c55" alt="Description" style="width: 60%;">  
  
- Foreground & Backgroung Image ( Type : Mb8 )   
<img src="https://github.com/user-attachments/assets/9aaebff7-11f5-4e77-a045-f62669373bf4" alt="Description" style="width: 60%;">  
<hr style="border-top: 3px solid #bbb;">

## Histogram Equalization

<img src="https://github.com/user-attachments/assets/acdb0558-fe28-48f9-bb57-5828e10562ca" alt="Description" style="width: 60%;">  
<hr style="border-top: 3px solid #bbb;">

## Histogram Matching

<img src="https://github.com/user-attachments/assets/ad1a9370-ad8e-4c5f-8f57-a806b79340fe" alt="Description" style="width: 60%;">  
<img src="https://github.com/user-attachments/assets/06123990-7a8b-4b9d-94d1-f29e549ed2fa" alt="Description" style="width: 60%;">  
<hr style="border-top: 3px solid #bbb;">


## Hough Transform

<img src="https://github.com/user-attachments/assets/5fe7b459-476d-454f-8292-a2a916a1f356" alt="Description" style="width: 60%;">  
<hr style="border-top: 3px solid #bbb;">

## Generalized Hough Transform 

<img src="https://github.com/user-attachments/assets/fde66e09-d7ec-4309-a759-fa2e39693c7b" alt="Description" style="width: 60%;">  
<hr style="border-top: 3px solid #bbb;">

## Optical Flow

Select consecutive images, calculate the optical flow, and display it on the screen.  
![image](https://github.com/user-attachments/assets/68bbf2af-1992-4517-a547-fb0abe7fad5f)
![image](https://github.com/user-attachments/assets/a29b65a0-7b47-4943-a1ca-189f6628f11f)
![image](https://github.com/user-attachments/assets/6534529e-2636-49a5-b3e0-063b72bbcf0b)
<hr style="border-top: 3px solid #bbb;">
