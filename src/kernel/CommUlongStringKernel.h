#ifndef _COMMULONGSTRINGKERNEL_H___
#define _COMMULONGSTRINGKERNEL_H___

#include "lib/common.h"
#include "lib/Mathmatics.h"
#include "lib/DynArray.h"
#include "kernel/StringKernel.h"

class CCommUlongStringKernel: public CStringKernel<ULONG>
{
	public:
		CCommUlongStringKernel(LONG size, bool use_sign, ENormalizationType normalization_=FULL_NORMALIZATION );
		~CCommUlongStringKernel();

		virtual bool init(CFeatures* l, CFeatures* r, bool do_init);
		virtual void cleanup();

		/// load and save kernel init_data
		bool load_init(FILE* src);
		bool save_init(FILE* dest);

		// return what type of kernel we are Linear,Polynomial, Gaussian,...
		virtual EKernelType get_kernel_type() { return K_COMMULONGSTRING; }

		// return the name of a kernel
		virtual const CHAR* get_name() { return "CommUlongString"; }

		virtual bool init_optimization(INT count, INT* IDX, REAL* weights);
		virtual bool delete_optimization();
		virtual REAL compute_optimized(INT idx);

		inline void merge_dictionaries(INT &t, INT j, INT &k, ULONG* vec, ULONG* dic, REAL* dic_weights, REAL weight, INT vec_idx, INT len, ENormalizationType normalization)
		{
			while (k<dictionary.get_num_elements() && dictionary[k] < vec[j-1])
			{
				dic[t]=dictionary[k];
				dic_weights[t]=dictionary_weights[k];
				t++;
				k++;
			}

			if (k<dictionary.get_num_elements() && dictionary[k]==vec[j-1])
			{
				dic[t]=vec[j-1];
				dic_weights[t]=dictionary_weights[k]+normalize_weight(weight, vec_idx, len, normalization);
				k++;
			}
			else
			{
				dic[t]=vec[j-1];
				dic_weights[t]=normalize_weight(weight, vec_idx, len, normalization);
			}
			t++;
		}

		virtual void add_to_normal(INT idx, REAL weight);
		virtual void clear_normal();

		virtual void remove_lhs();
		virtual void remove_rhs();

		inline virtual EFeatureType get_feature_type() { return F_ULONG; }

		void get_dictionary(INT &dsize, ULONG*& dict, REAL*& dweights) 
		{
			dsize=dictionary.get_num_elements();
			dict=dictionary.get_array();
			dweights = dictionary_weights.get_array();
		}

	protected:
		/// compute kernel function for features a and b
		/// idx_{a,b} denote the index of the feature vectors
		/// in the corresponding feature object
		REAL compute(INT idx_a, INT idx_b);

		inline REAL normalize_weight(REAL value, INT seq_num, INT seq_len, ENormalizationType normalization)
		{
			switch (normalization)
			{
				case NO_NORMALIZATION:
					return value;
					break;
				case SQRT_NORMALIZATION:
					return value/sqrt(sqrtdiag_lhs[seq_num]);
					break;
				case FULL_NORMALIZATION:
					return value/sqrtdiag_lhs[seq_num];
					break;
				case SQRTLEN_NORMALIZATION:
					return value/sqrt(sqrt(seq_len));
					break;
				case LEN_NORMALIZATION:
					return value/sqrt(seq_len);
					break;
				case SQLEN_NORMALIZATION:
					return value/seq_len;
					break;
				default:
					ASSERT(0);
			}
			return -CMath::INFTY;
		}

	protected:
		REAL* sqrtdiag_lhs;
		REAL* sqrtdiag_rhs;

		bool initialized;

		CDynamicArray<ULONG> dictionary;
		CDynamicArray<REAL> dictionary_weights;

		bool use_sign;
		ENormalizationType normalization;
};
#endif
