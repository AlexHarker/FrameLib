
/*
 *  ibuffer_access.h
 *
 *	This header file provides an interface for accessing and interpolating samples from an ibuffer (or standard MSP buffer).
 *	You should also compile ibufffer_access.c in the project
 *
 *	Various kinds of interpolation are supported along with both SIMD calculation and both 32 bit and 64 bit floating point precision.
 *
 *	All pointers used with the SIMD routines should be 16-byte aligned and a multiple of 4 values wide (regardless of type).
 *  The SIMD routines currently always write a multiple of 4 values, although any extra values will be zero.
 *	Pointers for non-SIMD routines can be arbitrarily aligned.
 *
 *	The init routine should be called in an objects main function to setup the necessary variables.
 *	Use the ibuffer__get_ptr / ibuffer_info / ibuffer_sample_rate functions to get info about the ibuffer (or buffer).
 *	The buffer inuse field should be incremented and decremented beoore and after use using ibuffer_increment_inuse and ibuffer_decrement_inuse.
 *
 *
 *	The main routines for accessing samples are then:
 *
 *	ibuffer_get_samp							- get a single sample
 *	ibuffer_get_samps							- get a number of consecutive samples
 *	ibuffer_get_samps_rev						- get a number of consecutive samples in reverse order
 *
 *	offsets is a pointer to the sample offsets
 *	fracts is a pointer to the subsample (fractional) offsets
 *	temp is a pointer to an array of non-aliasing 16-byte aligned pointers each of which is the same size as the required output.
 *		- the array should be two pointers wide for linear interpolation and four for cubic interpolation
 *	n_samps is the number of samples required.
 *	n_chans is the number of channels in the buffer (as returned by a call the ibuffer_info).
 *	chan is the channel you wish to access.
 *	format is the format of the buffer (as returned by a call the ibuffer_info).
 *
 *	Most of these routines have a mul input to multiply by a constant value.
 *	Other variables should be self-explanatory 
 *	
 *  Copyright 2010-13 Alex Harker. All rights reserved.
 *
 */


#ifndef _IBUFFER_ACCESS_
#define _IBUFFER_ACCESS_

#include "ibuffer.h"
#include "AH_VectorOps.h"

extern t_symbol *ps_none;
extern t_symbol *ps_linear;
extern t_symbol *ps_bspline;
extern t_symbol *ps_hermite;
extern t_symbol *ps_lagrange;
extern t_symbol *ps_buffer;
extern t_symbol *ps_ibuffer;

enum InterpType
{
    kInterpNone, kInterpLinear, kInterpCubicHermite, kInterpCubicLagrange, kInterpCubicBSpline
};

struct ibuffer_data {
    
    ibuffer_data() : samples(NULL), length(0), num_chans(0), format(0) {}
    
    void *samples;
    long length;
    long num_chans;
    long format;
};

// Call in main routine to initialise buffer symbols

void ibuffer_init();

#ifdef __APPLE__

// Get ibuffer and related info (note that the sample rate is in a separate call, as it is not required info for other routines

static __inline void *ibuffer_get_ptr (t_symbol *s) FORCE_INLINE;
static __inline const ibuffer_data ibuffer_info(void *thebuffer) FORCE_INLINE;
static __inline double ibuffer_sample_rate (void *thebuffer) FORCE_INLINE;

// Increment / decrement buffer inuse pointers

static __inline void ibuffer_increment_inuse(void *thebuffer) FORCE_INLINE;
static __inline void ibuffer_decrement_inuse(void *thebuffer) FORCE_INLINE;


// Calculate an offset to the samps pointer (accounting for sample interleaving and sample format)

static __inline void *ibuffer_offset(void *samps, intptr_t offset, long n_chans, long format) FORCE_INLINE;

#endif

// Get the value of an individual sample

static __inline float ibuffer_float_get_samp(const ibuffer_data& data, intptr_t offset, long chan)  FORCE_INLINE;
static __inline double ibuffer_double_get_samp(const ibuffer_data& data, intptr_t offset, long chan)  FORCE_INLINE;

// Get consecutive samples (and in reverse)

void ibuffer_get_samps(const ibuffer_data& data, float *out, intptr_t offset, intptr_t n_samps, long chan, bool reverse);
void ibuffer_get_samps(const ibuffer_data& data, double *out, intptr_t offset, intptr_t n_samps, long chan, bool reverse);

// Read with various forms of interpolation

void ibuffer_read(const ibuffer_data& data, float *out, intptr_t *offsets, float *fracts, intptr_t n_samps, long chan, float mul, InterpType interp);
void ibuffer_read(const ibuffer_data& data, double *out, intptr_t *offsets, double *fracts, intptr_t n_samps, long chan, double mul, InterpType interp);

        
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// Get ibuffer and related info //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static __inline void *ibuffer_get_ptr(t_symbol *s) FORCE_INLINE_DEFINITION
{
	t_object *b;
	
	if (!s) 
		return 0;
	
	b = s->s_thing;
	
	if (b && (ob_sym(b) == ps_ibuffer || ob_sym(b) == ps_buffer))
		return b;
	else 
		return 0;
}



static __inline const ibuffer_data ibuffer_info(void *thebuffer) FORCE_INLINE_DEFINITION
{
    ibuffer_data data;
                 
	if (!thebuffer) 
		return data;
	
	if (ob_sym(thebuffer) == ps_buffer)
	{
		t_buffer *buffer = (t_buffer *) thebuffer;
		if (buffer->b_valid)
		{
			data.samples = (void *) buffer->b_samples;
			data.length = buffer->b_frames;
			data.num_chans = buffer->b_nchans;
			data.format = PCM_FLOAT;
		}
	}
	else
	{
		t_ibuffer *buffer = (t_ibuffer *) thebuffer;
		if (buffer->valid)
		{
			data.samples =  buffer->samples;
			data.length = buffer->frames;
			data.num_chans = buffer->channels;
			data.format = buffer->format;
		}
	}

	return data;
}


static __inline double ibuffer_sample_rate(void *thebuffer) FORCE_INLINE_DEFINITION
{
	if (ob_sym(thebuffer) == ps_buffer)
		return (double) ((t_buffer *)thebuffer)->b_sr;
	else
		return (double) ((t_ibuffer *)thebuffer)->sr;		
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// Inc /dec inuse counts /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static __inline void ibuffer_increment_inuse (void *thebuffer) FORCE_INLINE_DEFINITION
{
	if (ob_sym(thebuffer) == ps_buffer)
		ATOMIC_INCREMENT(&((t_buffer *)thebuffer)->b_inuse);
	else
        ATOMIC_INCREMENT(&((t_ibuffer *)thebuffer)->inuse);
}


static __inline void ibuffer_decrement_inuse (void *thebuffer) FORCE_INLINE_DEFINITION
{
	if (ob_sym(thebuffer) == ps_buffer)
		ATOMIC_DECREMENT(&((t_buffer *)thebuffer)->b_inuse);
	else
		ATOMIC_DECREMENT(&((t_ibuffer *)thebuffer)->inuse);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// Get individual samples /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static __inline float ibuffer_float_get_samp(const ibuffer_data& data, intptr_t offset, long chan) FORCE_INLINE_DEFINITION
{	
	uint32_t sampleint;
	
	switch (data.format)
	{
		case PCM_INT_16:
			sampleint = ((* (uint16_t *) (((uint16_t *) data.samples) + chan + (offset * data.num_chans) ) ) << 16) & MASK_16_BIT;
			return (float) *((int32_t *) &sampleint) * TWO_POW_31_RECIP;			
			
		case PCM_INT_24:
			sampleint = * ( (uint32_t *) ( ((uint8_t *) data.samples) + (3 * (chan + (offset * data.num_chans))) - 1 )) & MASK_24_BIT;
			return (float) *((int32_t *) &sampleint) * TWO_POW_31_RECIP;		
			
		case PCM_INT_32:
			return (float) ( *( ((int32_t *) data.samples) + chan + (offset * data.num_chans) ) ) * TWO_POW_31_RECIP;
			
		case PCM_FLOAT:
			return *( ((float *) data.samples) + chan + (offset * data.num_chans) );
	}
	
	return 0.f;
}

static __inline double ibuffer_double_get_samp(const ibuffer_data& data, intptr_t offset, long chan) FORCE_INLINE_DEFINITION
{	
	uint32_t sampleint;
	
	switch (data.format)
	{
		case PCM_INT_16:
			sampleint = ((* (uint16_t *) (((uint16_t *) data.samples) + chan + (offset * data.num_chans) ) ) << 16) & MASK_16_BIT;
			return (double) *((int32_t *) &sampleint) * TWO_POW_31_RECIP_DOUBLE;			
			
		case PCM_INT_24:
			sampleint = * ( (uint32_t *) ( ((uint8_t *) data.samples) + (3 * (chan + (offset * data.num_chans))) - 1 )) & MASK_24_BIT;
			return (double) *((int32_t *) &sampleint) * TWO_POW_31_RECIP_DOUBLE;		
			
		case PCM_INT_32:
			return (double) ( *( ((int32_t *) data.samples) + chan + (offset * data.num_chans) ) ) * TWO_POW_31_RECIP_DOUBLE;
			
		case PCM_FLOAT:
			return (double) *( ((float *) data.samples) + chan + (offset * data.num_chans) );
	}
	
	return 0.0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// Add a fixed offset to the samps pointer ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static __inline void *ibuffer_offset(void *samps, intptr_t offset, long n_chans, long format) FORCE_INLINE_DEFINITION
{
	int8_t *samps_temp = (int8_t *) samps;
	
	switch (format)
	{		
		case PCM_INT_16:
			return samps_temp + (offset * 2 * n_chans);
			
		case PCM_INT_24:
			return samps_temp + (offset * 3 * n_chans);
			
		case PCM_INT_32:
			return samps_temp + (offset * 4 * n_chans);
			
		case PCM_FLOAT:
			return samps_temp + (offset * 4 * n_chans);
	}
	
	return 0;
}

#endif	/* _IBUFFER_ACCESS_ */
