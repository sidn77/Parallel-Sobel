# Parallelized Implementation of Sobel Operator

My experiment with implementing a parallelized version of Sobel Operator.

## Introduction
The Sobel operator, sometimes called the Sobel–Feldman operator or Sobel filter, is used in image
processing and computer vision, particularly within edge detection algorithms where it creates an
image emphasising edges. It is named after Irwin Sobel and Gary Feldman, colleagues at the
Stanford Artificial Intelligence Laboratory (SAIL). It was co-developed with Gary Feldman at
SAIL.
Sobel and Feldman presented the idea of an "Isotropic 3x3 Image Gradient Operator" at a talk at
SAIL in 1968. Technically, it is a discrete differentiation operator, computing an approximation of
the gradient of the image intensity function.
At each point in the image, the result of the Sobel–Feldman operator is either the corresponding
gradient vector or the norm of this vector. The Sobel–Feldman operator is based on convolving the
image with a small, separable, and integer-valued filter in the horizontal and vertical directions and
is therefore relatively inexpensive in terms of computations.
On the other hand, the gradient approximation that it produces is relatively crude, in particular for
high-frequency variations in the image.
--Formulation--
The operator uses two 3×3 kernels which are convolved with the original image to calculate
approximations of the derivatives – one for horizontal changes, and one for vertical. If we define A
as the source image, and Gx and Gy are two images which at each point contain the horizontal and
vertical derivative approximations respectively, the computations are as follows:

    G(x) = | +1, 0, -1 | and G(y) = | +1, +2, +1 |
           | +2, 0, -2 |            | 0, 0, 0    |
           | +1, 0, -1 |            | -1, -2, -1 |


where * here denotes the 2-dimensional signal processing convolution operation.
Since the Sobel kernels can be decomposed as the products of an averaging and a differentiation
kernel, they compute the gradient with smoothing. For example, G(x) can be written as

    | +1, 0, -1 |   | 1 | 
    | +2, 0, -2 | = | 2 | | +1, 0, -1 |
    | +1, 0, -1 |   | 1 |

The x-coordinate is defined here as increasing in the "right"-direction, and the y-coordinate is
defined as increasing in the "down"-direction. At each point in the image, the resulting gradient
approximations can be combined to give the gradient magnitude, using:
      
G = sqrt(G(x)^2 + G(y)^2)

Using this information, we can also calculate the gradient's direction:

theta = atan(G(x) / G(y))

where, for example, theta is 0 for a vertical edge which is lighter on the right side.

## MPI
Message Passing Interface (MPI) is a standardized and portable message-passing system designed
by a group of researchers from academia and industry to function on a wide variety of parallel
computing architectures.
The standard defines the syntax and semantics of a core of library routines useful to a wide range of
users writing portable message-passing programs in C, C++, and Fortran.
There are several well-tested and efficient implementations of MPI, many of which are open-source
or in the public domain. These fostered the development of a parallel software industry, and
encouraged development of portable and scalable large-scale parallel applications.
